#include <queue>
#include <fstream>
#include "Cdcl.h"
using namespace std;

CDCL::CDCL(CNF* _cnf) {
	cnf = new CNF(*_cnf);
	dependency_graph = new Graph();
	parent_decision = nullptr;
}

bool CDCL::SATISFIABLE() {
	result* res = CDCLAlgorithm(cnf);
	cnf = res->model_cnf;
	return res->sat;
}

/**
 * Returns the first unassigned variable or nullptr if all variables are assigned
 */
Variable* CDCL::find_first_unassigned(VariableSet* vars) {
	for (auto v : *vars) {
		if (!v->get_assigned()) {
			return v;
		}
	}
	return nullptr;
}

/**
 * Helper function to great a standard literal
 */
StandardLiteral* CDCL::create_standard_literal(unsigned name, bool sign) {
	auto* l = new StandardLiteral(name, sign, parent_decision);
	if (parent_decision != nullptr) {
		parent_decision->add_implies(l);
	}
	return l;
}

/**
 * Helper function to great a decision literal
 */
DecisionLiteral* CDCL::create_decision_literal(unsigned name, bool sign) {
	auto* l = new DecisionLiteral(name, sign);
	if (parent_decision != nullptr) {
		parent_decision->add_child(l);
		l->parents.insert((Literal*)parent_decision);
	}
	return l;
}

/**
 * Checks if all clauses are true
 */
bool CDCL::ALL_CLAUSES_ARE_TRUE(ClauseSet* clauses) {
	for (auto clause : *clauses) {
		if (!clause->is_true()) {
			return false;
		}
	}
	return true;
}

/**
 * Checks if there exists a clause that is false
 */
bool CDCL::ONE_CLAUSE_IS_FALSE(ClauseSet* clauses) {
	for (auto clause : *clauses) {
		if (clause->is_false()) {
			return true;
		}
	}
	return false;
}

/**
 * sets a given variable to a given value in the whole cnf
 */
void CDCL::set_variable_value(CNF* cnf, Variable* var, bool value) {
	for (auto variable : *cnf->get_vars()) {
		if (variable->get_name() == var->get_name()) {
			variable->set_value(value);
			variable->set_assigned(true);
		}
	}
}

void CDCL::unset_variable_value(CNF* cnf, Variable* var) {
	for (auto variable : *cnf->get_vars()) {
		if (variable->get_name() == var->get_name()) {
			variable->set_assigned(false);
		}
	}
}

/**
 * Adds the neccessary dependency edges to the graph after the unit clause rule was applied
 *
 * The edges are: per previously removed variable in the clause add an edge from the negation
 * of that variable. The negation of the variable is guaranteed to be in the graph.
 * @param name the name of the unit variable
 * @param sign the sign of the unit variable
 * @param clause the unit clause itself
 */

void CDCL::add_dependency_edges_to_graph(unsigned name, bool sign, Clause* clause) {
	Literal* l = create_standard_literal(name, sign);
	dependency_graph->add_node(l);
	for (auto v : *clause->get_vars()) {
		if (!(v->get_sign() == sign && v->get_name() == name)) {
			Literal* parent = dependency_graph->find_negation(v);
			if (parent != nullptr) {
				if (parent->name != name) {
					parent->children.insert(l);
					l->parents.insert(parent);
				}
			}
		}
	}
}

/**
 * Goes through the cnf formula and looks for a unit clause
 * @param cnf the cnf object that represents the original formula and a partial model (var->value assignment)
 * @return the unit clause if it found one, nullptr otherwise
 */

pair<Clause*, Variable*> CDCL::FIND_UNIT_CLAUSE(CNF* cnf) {
	for (auto c : *cnf->get_clauses()) {
		if (!c->is_true()) {
			long count = c->get_vars()->size();
			Variable* unasigned = nullptr;
			for (auto cv : *(c->get_vars())) {
				if (cv->get_assigned()) {
					count--;
				}
				else {
					unasigned = cv;
				}
			}
			if (count == 1) {
				return make_pair(c, unasigned);
			}
		}
	}
	return make_pair((Clause*) nullptr, (Variable*) nullptr);
}

result* CDCL::CDCLAlgorithm(CNF* cnf) {
	Variable* var;
	Clause* clause;
	ClauseSet* clauses = cnf->get_clauses();
	VariableSet* vars = cnf->get_vars();
	if (dependency_graph->has_conflict && parent_decision == nullptr) {
		return new result(false, nullptr);
	}
	if (dependency_graph->has_conflict) {
		CNF* new_cnf = conflict_resolution();
		return CDCL::CDCLAlgorithm(new_cnf);
	}
	if (ALL_CLAUSES_ARE_TRUE(clauses))
		return new result(true, cnf);
	if (ONE_CLAUSE_IS_FALSE(clauses)) {
		if (parent_decision == nullptr) {
			return new result(false, nullptr);
		}
	}
	pair<Clause*, Variable*> uc = FIND_UNIT_CLAUSE(cnf);
	clause = uc.first;
	var = uc.second;
	if (clause != nullptr) {
		set_variable_value(cnf, var, var->get_sign());
		add_dependency_edges_to_graph(var->get_name(), var->get_sign(), clause);
		if (ONE_CLAUSE_IS_FALSE(cnf->get_clauses())) {
			Clause* false_clause = nullptr;
			for (auto c : *clauses) {
				if (c->is_false()) {
					false_clause = c;
				}
			}
			add_dependency_edges_to_graph(var->get_name(), !var->get_sign(), false_clause);
		}
		return CDCLAlgorithm(cnf);
	}
	var = find_first_unassigned(vars);
	if (var == nullptr) {
		return new result(false, nullptr);
	}
	else {
		DecisionLiteral* l = create_decision_literal(var->get_name(), var->get_sign());
		dependency_graph->add_node(l);
		parent_decision = l;
		set_variable_value(cnf, var, var->get_sign());
		return CDCLAlgorithm(cnf);
	}
}

CNF* CDCL::get_cnf() {
	return cnf;
}

/**
 * Applies conflict resolution to the dependency graph -> back jumping and clause learning
 */
CNF* CDCL::conflict_resolution() {
	auto* p = (StandardLiteral*)dependency_graph->find(new Variable(true, false, dependency_graph->conflict));
	auto* n = (StandardLiteral*)dependency_graph->find(new Variable(false, false, dependency_graph->conflict));

	Clause* newClause = learn_clause(p, n);
	cnf->add_clause(newClause);

	// identify the relevant decision literals
	LiteralSet relevant;
	relevant.insert((Literal*)p->implied_by);
	relevant.insert((Literal*)n->implied_by);

	jump_back(relevant);
	dependency_graph->has_conflict = false;
	return cnf;
}

/**
 * Cuts the dependency graph and construct a new learned clause
 * @param positive_literal the positive literal of the conflict
 * @param negative_literal the negative literal of the conflict
 * @return a new clause learned from the conflict
 */
Clause* CDCL::learn_clause(StandardLiteral* positive_literal, StandardLiteral* negative_literal) {
	LiteralSet parents;
	parents.insert(positive_literal->parents.begin(), positive_literal->parents.end());
	parents.insert(negative_literal->parents.begin(), negative_literal->parents.end());
	VariableSet new_clause_variables;
	LiteralSet decisionParents;
	queue<Literal*> workList;
	for (auto parent : parents) {
		workList.push(parent);
	}
	while (!workList.empty()) {
		Literal* e = workList.front();
		workList.pop();
		if (e->type == DECISION) {
			// NOTE: this step is done to get rid of duplicates
			decisionParents.insert(e);
		}
		else {
			for (auto parent : e->parents) {
				workList.push(parent);
			}
		}
	}

	for (auto parent : decisionParents) {
		new_clause_variables.push_back(new Variable(!parent->sign, false, parent->name));
	}
	Clause* result = new Clause(new_clause_variables);
	return result;
}

/**
 * Jumps back in the dependency graph
 * Here we pop one relevant literal and as many others as possible (non-relevant ones)
 */
void CDCL::jump_back(LiteralSet relevant_literals) {
	bool one_popped = false;
	while (parent_decision != nullptr) {
		if (relevant_literals.find(parent_decision) != relevant_literals.end()) {
			if (one_popped) {
				break;
			}
			else {
				one_popped = true;
			}
		}
		remove_all_consequences(parent_decision);
		if (parent_decision->parents.size() == 1) {
			parent_decision = (DecisionLiteral*) * (parent_decision->parents.begin());
		}
		else {
			parent_decision = nullptr;
		}
	}
	dependency_graph->free_freeable_nodes();
}

void CDCL::remove_all_consequences(DecisionLiteral* literal) {
	for (auto consequence : literal->implies) {
		CDCL::unset_variable_value(cnf, new Variable(false, false, consequence->name));
	}
	unset_variable_value(cnf, new Variable(false, false, literal->name));
	dependency_graph->remove_node(literal);
}

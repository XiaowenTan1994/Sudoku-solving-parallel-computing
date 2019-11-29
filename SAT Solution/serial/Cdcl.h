#ifndef SAT_SOLVER_CDCL_H
#define SAT_SOLVER_CDCL_H

#include <unordered_map>
#include "Cnf.h"
#include "Clause.h"
#include "InternalTypes.h"
#include "CdclDependencyGraph.h"

class result {
public:
	bool sat;
	CNF* model_cnf;
	result(bool _sat, CNF* _model_cnf) {
		sat = _sat;
		model_cnf = _model_cnf;
	}
};

class CDCL {
private:
    CNF *cnf;
    Graph *dependency_graph;
    DecisionLiteral *parent_decision;
    StandardLiteral *create_standard_literal(unsigned int name, bool sign);
    DecisionLiteral *create_decision_literal(unsigned int name, bool sign);
    std::pair<Clause *, Variable *> FIND_UNIT_CLAUSE(CNF *cnf);
    void add_dependency_edges_to_graph(unsigned int name, bool sign, Clause *clause);
    CNF* conflict_resolution();
	Variable* find_first_unassigned(VariableSet* vars);
	bool ALL_CLAUSES_ARE_TRUE(ClauseSet* clauses);
	bool ONE_CLAUSE_IS_FALSE(ClauseSet* clauses);
	void set_variable_value(CNF* cnf, Variable* var, bool value);
	void unset_variable_value(CNF* cnf, Variable* var);
    Clause *learn_clause(StandardLiteral *positive_literal, StandardLiteral *negative_literal);
    void jump_back(LiteralSet relevant_literals);
    void remove_all_consequences(DecisionLiteral *literal);
public:
    explicit CDCL(CNF *_cnf);
    result *CDCLAlgorithm(CNF *cnf);
    bool SATISFIABLE() ;
    CNF *get_cnf() ;
	static void output(VariableSet* vars);
};


#endif //SAT_SOLVER_CDCL_H

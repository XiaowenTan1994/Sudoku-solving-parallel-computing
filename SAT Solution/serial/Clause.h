#ifndef CLAUSE_H
#define CLAUSE_H

#include "Variable.h"
#include "InternalTypes.h"

/**
 * Represents a clause in a CNF formula
 */
class Clause {
private:
    VariableSet var;		/* a set of variables */
public:
    explicit Clause(VariableSet _var);
    bool is_true();
    bool is_false();
    VariableSet* get_vars();
    std::string to_string();
};

#endif // CLAUSE_H

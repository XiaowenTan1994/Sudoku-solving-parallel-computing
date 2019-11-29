#include "Model.h"

// creates empty state
Model::Model(){}

// creates state with variables and its size
Model::Model(unsigned int * variables, unsigned int size){
    this->set_variables(variables, size);
}

// gets variables
unsigned int * Model::get_variables(){
    return this->variables;
}

// gets size of variables
unsigned int Model::get_size(){
    return this->size;
}

// sets variables and its size
void Model::set_variables(unsigned int * variables, unsigned int size){
    this->variables = variables;
    this->size = size;
}

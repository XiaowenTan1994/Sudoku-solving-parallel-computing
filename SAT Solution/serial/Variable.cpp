#include "Variable.h"

#include <iostream>

//constructor
Variable::Variable(bool _sign, bool _value, unsigned _name)
        : sign(_sign), value(_value), name(_name), assigned(false) {}

//copy construcor
Variable::Variable(const Variable &v) {						/* copy constructor */
    sign = v.get_sign();
    value = v.get_value();
    assigned = v.get_assigned();
    name = v.get_name();
}

// gets value of sign
bool Variable::get_sign() const {
    return sign;
}

//gets value
bool Variable::get_value() const {
    return value;
}

//gets value of name
unsigned Variable::get_name() const {
    return name;
}

//indicates whether is variable assigned
bool Variable::get_assigned() const {
    return assigned;
}

//gets real value
bool Variable::get_real_value() const{
    return sign ? value : !value;
}

//sets value
void Variable::set_value(bool _value) {
    value = _value;
}

//set whether is variable assigned
void Variable::set_assigned(bool _assigned) {
    assigned = _assigned;
}

//sets sign
void Variable::set_sign(bool _sign){
    sign = _sign;
}

std::string Variable::to_string() {
    return sign ? std::to_string(name) : "!" + std::to_string(name);
}


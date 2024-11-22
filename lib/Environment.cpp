#include "Environment.h"

Environment::Environment(std::shared_ptr<Environment> parent): parent(parent) {}

void Environment::setVariable(const std::string &name, VariantValue value) {
    if (variables.find(name) == variables.end()) {
         if(parent)
            throw std::runtime_error("Undefined variable: " + name);
        parent->setVariable(name, value);
        return;
    }
	variables[name].value = value;
}

void Environment::declareVariable(const std::string &name, ValueType type,
								  VariantValue value) {
    if(variables.find(name) != variables.end())
        throw std::runtime_error("Variable previously decleared: " + name);
        
    variables.emplace(name, Variable(value, type));
}

Variable Environment::getVariable(const std::string& name) const {
    if (variables.find(name) == variables.end()) {
        if(parent)
            throw std::runtime_error("Undefined variable: " + name);
        return parent->getVariable(name);
    }
    return variables.at(name);
}

bool Environment::hasVariable(const std::string& name) const {
    if (variables.find(name) == variables.end()) {
        if(parent)
            return false;
        return parent->hasVariable(name);
    }
    return true;
}
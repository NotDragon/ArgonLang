#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include "Value.h"

class Environment {
private:
    std::unordered_map<std::string, Variable> variables;
    std::shared_ptr<Environment> parent;

public:
    Environment(std::shared_ptr<Environment> parent);
    
    void setVariable(const std::string& name, VariantValue value);
    void declareVariable(const std::string& name, ValueType type, VariantValue value);

    Variable getVariable(const std::string& name) const;

    bool hasVariable(const std::string& name) const;
};

#endif // ENVIRONMENT_H
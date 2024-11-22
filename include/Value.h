#ifndef VALUE_H
#define VALUE_H

#include <variant>
#include <string>

using VariantValue = std::variant<int, double, std::string, bool>;

enum class ValueType {
    INTEGRAL = 0,
    DECIMAL,
    STRING,
    BOOLEAN
};

class Variable {
public:
    VariantValue value;
    ValueType type;
    Variable(VariantValue value, ValueType type);
    Variable();
};

class Value {
public:
    virtual ~Value() = default;
    virtual ValueType getType() const = 0;
};

class IntegralValue: Value {
public:
    int value;
    IntegralValue(int value);
    ValueType getType() const override;
};

class DecimalValue: Value {
public:
    double value;
    DecimalValue(double value);
    ValueType getType() const override;
};

class StringValue: Value {
public:
    std::string value;
    StringValue(std::string value);
    ValueType getType() const override;
};

class BooleanValue: Value {
public:
    bool value;
    BooleanValue(bool value);
    ValueType getType() const override;
};

#endif // VALUE_H
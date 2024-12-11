#include "frontend/Value.h"

Variable::Variable(VariantValue value, ValueType type): value(value), type(type) {}

Variable::Variable() {}

IntegralValue::IntegralValue(int value): value(value) {}
DecimalValue::DecimalValue(double value) : value(value) {}
StringValue::StringValue(std::string value): value(value) {}
BooleanValue::BooleanValue(bool value): value(value) {}


ValueType IntegralValue::getType() const { return ValueType::INTEGRAL; }
ValueType DecimalValue::getType() const { return ValueType::DECIMAL; }
ValueType StringValue::getType() const { return ValueType::STRING; }
ValueType BooleanValue::getType() const { return ValueType::BOOLEAN; }
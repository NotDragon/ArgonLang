//
// Created by User on 31/12/2024.
//
#include "frontend/AnalysisValue.h"
using namespace ArgonLang;

ValueType StringValue::getValueType() {
	return ValueType::String;
}

ValueType CharValue::getValueType() {
	return ValueType::Char;
}

ValueType IntegralValue::getValueType() {
	return ValueType::Integral;
}

ValueType FloatValue::getValueType() {
	return ValueType::Float;
}

ValueType BooleanValue::getValueType() {
	return ValueType::Bool;
}
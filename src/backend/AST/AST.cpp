#include "backend/AST.h"

std::string ArgonLang::primitiveTypeToString(PrimitiveType type) {
	switch (type) {
		case INT8: return "i8";
		case INT16: return "i16";
		case INT32: return "i32";
		case INT64: return "i64";
		case INT128: return "i128";
		case FLOAT32: return "f32";
		case FLOAT64: return "f64";
		case FLOAT128: return "f128";
	}
	return "";
}

ArgonLang::PrimitiveType ArgonLang::determineIntegerType(const std::string& value) {
	if (value.ends_with("i8")) return PrimitiveType::INT8;
	if (value.ends_with("i16")) return PrimitiveType::INT16;
	if (value.ends_with("i32")) return PrimitiveType::INT32;
	if (value.ends_with("i64")) return PrimitiveType::INT64;
	if (value.ends_with("i128")) return PrimitiveType::INT128;

	return PrimitiveType::INT32;
}

ArgonLang::PrimitiveType ArgonLang::determineFloatType(const std::string& value) {
	if (value.ends_with("f32")) return PrimitiveType::FLOAT32;
	if (value.ends_with("f64")) return PrimitiveType::FLOAT64;
	if (value.ends_with("f128")) return PrimitiveType::FLOAT128;

	return PrimitiveType::FLOAT32;
}
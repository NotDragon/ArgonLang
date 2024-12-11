#include "backend/Parser.h"

std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseGenericType() {
	return std::unique_ptr<TypeNode>();
}

std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseUnionType() {
	return std::unique_ptr<TypeNode>();
}

std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseArrayType() {
	return std::unique_ptr<TypeNode>();
}
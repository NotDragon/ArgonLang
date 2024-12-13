#include "backend/Parser.h"

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseType() {
	advance();
	return { std::make_unique<IdentifierTypeNode>("") };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseGenericType() {
	return std::unique_ptr<TypeNode>();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseUnionType() {
	return std::unique_ptr<TypeNode>();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseArrayType() {
	return std::unique_ptr<TypeNode>();
}
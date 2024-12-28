#include "backend/Parser.h"

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseType() {
	return parseSumType();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseIdentifierType() {
	Result<Token> left = expect(Token::Identifier, "Expected Identifier");
	if(left.hasError()) return { left, Trace("", ASTNodeType::IdentifierType, left.getValue().position) };

	return { std::make_unique<IdentifierTypeNode>(left.getValue().value) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parsePrefixedType() {
	if(peek().type != Token::Multiply && peek().type != Token::Ownership) return parseIdentifierType();

	return std::unique_ptr<TypeNode>();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseGenericType() {
	Result<std::unique_ptr<TypeNode>> left = parsePrefixedType();
	if(peek().type != Token::Less || left.hasError()) return left;

	Result<Token> less = advance();
	if (less.hasError()) return {less, Trace("", ASTNodeType::GenericType, less.getValue().position)};

	std::vector<std::unique_ptr<TypeNode>> args;
	while(peek().type != Token::Greater) {
		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> arg = parseType();
		if (arg.hasError()) return {std::move(arg), Trace("", ASTNodeType::GenericType, pos)};

		args.push_back(arg.moveValue());

		if (peek().type == Token::Greater) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' or '>'");
		if (comma.hasError()) return {comma, Trace("", ASTNodeType::GenericType, comma.getValue().position)};
	}


	return { std::make_unique<GenericTypeNode>(left.moveValue(), std::move(args)) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseSumType() {
	Result<std::unique_ptr<TypeNode>> left = parseIntersectionType();

	return left;
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseIntersectionType() {
	Result<std::unique_ptr<TypeNode>> left = parseGenericType();

	return left;
}

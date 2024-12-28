#include "backend/Parser.h"

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseType() {
	return parseSumType();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseIdentifierType() {
	Result<Token> left = advance();
	if(left.hasError()) return { left, Trace("", ASTNodeType::IdentifierType, left.getValue().position) };

	if(left.getValue().type != Token::Identifier && left.getValue().type != Token::PrimitiveType)
		return { "Expected type", Trace("", ASTNodeType::IdentifierType, left.getValue().position) };

	return { std::make_unique<IdentifierTypeNode>(left.getValue().value) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parsePrefixedType() {
	PrefixedTypeNode::Prefix prefix;
	switch (peek().type) {
		case Token::Multiply:
			prefix = PrefixedTypeNode::Prefix::Pointer;
			break;
		case Token::Ownership:
			prefix = PrefixedTypeNode::Prefix::Owned;
			break;
		default:
			return parseIdentifierType();
	}

	Result<Token> pref = advance();
	if(pref.hasError()) return { pref, Trace("", ASTNodeType::PrefixedType, pref.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> base = parseIdentifierType();
	if(base.hasError()) return { std::move(base), Trace("", ASTNodeType::PrefixedType, pos) };

	return { std::make_unique<PrefixedTypeNode>(base.moveValue(), prefix) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseGenericType() {
	Result<std::unique_ptr<TypeNode>> base = parsePrefixedType();
	if(peek().type != Token::Less || base.hasError()) return base;

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

	Result<Token> greater = advance();
	if (greater.hasError()) return {greater, Trace("", ASTNodeType::GenericType, less.getValue().position)};

	return { std::make_unique<GenericTypeNode>(base.moveValue(), std::move(args)) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseSumType() {
	Result<std::unique_ptr<TypeNode>> left = parseIntersectionType();
	if(peek().type != Token::FilterRange || left.hasError()) return left;

	std::vector<std::unique_ptr<TypeNode>> types;
	types.push_back(left.moveValue());
	while(peek().type == Token::FilterRange) {
		Result<Token> orToken = advance();
		if (orToken.hasError()) return {orToken, Trace("", ASTNodeType::SumType, orToken.getValue().position)};

		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> right = parseIntersectionType();
		if (right.hasError()) return {std::move(right), Trace("", ASTNodeType::SumType, pos)};

		types.push_back(right.moveValue());
	}

	return { std::make_unique<SumTypeNode>(std::move(types)) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseIntersectionType() {
	Result<std::unique_ptr<TypeNode>> left = parseGenericType();
	if(peek().type != Token::MapRange || left.hasError()) return left;

	std::vector<std::unique_ptr<TypeNode>> types;
	types.push_back(left.moveValue());
	while(peek().type == Token::MapRange) {
		Result<Token> andToken = advance();
		if (andToken.hasError()) return {andToken, Trace("", ASTNodeType::IntersectionType, andToken.getValue().position)};

		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> right = parseGenericType();
		if (right.hasError()) return {std::move(right), Trace("", ASTNodeType::IntersectionType, pos)};

		types.push_back(right.moveValue());
	}

	return { std::make_unique<IntersectionTypeNode>(std::move(types)) };
}

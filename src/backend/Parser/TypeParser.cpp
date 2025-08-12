#include "backend/Parser.h"

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseType() {
	return parseSumType();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseIdentifierType() {
	Result<Token> left = advance();
	if(left.hasError()) {
		// Create a trace with current position if available
		Token::Position pos = (current > 0) ? tokens[current-1].position : Token::Position{0, 0};
		return { left.getErrorMsg(), left.getErrorNote(), Trace(ASTNodeType::IdentifierType, pos) };
	}

	if(left.getValue().type == Token::LeftParen) {
		Result<std::unique_ptr<TypeNode>> type = parseType();
		Result<Token> tokenError1 = expect(Token::RightParen, "Expected closing ')'");
		if(tokenError1.hasError()) {
			return { tokenError1.getErrorMsg(), tokenError1.getErrorNote(), Trace(ASTNodeType::IdentifierType, left.getValue().position) };
		}
		return type;
	}

	if(left.getValue().type != Token::Identifier && left.getValue().type != Token::PrimitiveType) {
		return { "Expected type", "", Trace(ASTNodeType::IdentifierType, left.getValue().position) };
	}

	if (peek().type == Token::Identifier || peek().type == Token::PrimitiveType) {
		return { "Expected '&' or '|' between types", "Did you mean to add '&' or '|'?", Trace(ASTNodeType::IntersectionType, left.getValue().position) };
	}

	return { std::make_unique<IdentifierTypeNode>(
			left.getValue().position, left.getValue().value) };
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
	if(pref.hasError()) {
		Token::Position pos = (current > 0) ? tokens[current-1].position : Token::Position{0, 0};
		return { pref.getErrorMsg(), pref.getErrorNote(), Trace(ASTNodeType::PrefixedType, pos) };
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> base = parseIdentifierType();
	if(base.hasError()) return { std::move(base), Trace(ASTNodeType::PrefixedType, pos) };

	return { std::make_unique<PrefixedTypeNode>(base.getValue()->position,  base.moveValue(), prefix) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseGenericType() {
	Result<std::unique_ptr<TypeNode>> base = parsePrefixedType();
	if(peek().type != Token::Less || base.hasError()) return base;
	Token::Position pos = peek().position;
	Result<Token> less = advance();
	if (less.hasError()) {
		return { less.getErrorMsg(), less.getErrorNote(), Trace(ASTNodeType::GenericType, pos) };
	}

	std::vector<std::unique_ptr<TypeNode>> args;
	while(peek().type != Token::Greater) {
		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> arg = parseType();
		if (arg.hasError()) return {std::move(arg), Trace(ASTNodeType::GenericType, pos)};

		args.push_back(arg.moveValue());

		if (peek().type == Token::Greater) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' or '>'");
		if (comma.hasError()) {
			Token::Position pos = (current > 0) ? tokens[current-1].position : Token::Position{0, 0};
			return { comma.getErrorMsg(), comma.getErrorNote(), Trace(ASTNodeType::GenericType, pos) };
		}
	}

	Result<Token> greater = advance();
	if (greater.hasError()) {
		Token::Position pos = (current > 0) ? tokens[current-1].position : Token::Position{0, 0};
		return { greater.getErrorMsg(), greater.getErrorNote(), Trace(ASTNodeType::GenericType, pos) };
	}

	return { std::make_unique<GenericTypeNode>(base.getValue()->position, base.moveValue(), std::move(args)) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseSumType() {
	Token::Position leftPos = peek().position;
	Result<std::unique_ptr<TypeNode>> left = parseIntersectionType();
	if(peek().type != Token::FilterRange || left.hasError()) return left;

	std::vector<std::unique_ptr<TypeNode>> types;
	types.push_back(left.moveValue());
	while(peek().type == Token::FilterRange) {
		Result<Token> orToken = advance();
		if (orToken.hasError()) {
			Token::Position pos = (current > 0) ? tokens[current-1].position : Token::Position{0, 0};
			return { orToken.getErrorMsg(), orToken.getErrorNote(), Trace(ASTNodeType::SumType, pos) };
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> right = parseIntersectionType();
		if (right.hasError()) return {std::move(right), Trace(ASTNodeType::SumType, pos)};

		types.push_back(right.moveValue());
	}

	return { std::make_unique<SumTypeNode>(leftPos, std::move(types)) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseIntersectionType() {
	Token::Position leftPos = peek().position;
	Result<std::unique_ptr<TypeNode>> left = parseGenericType();
	if(peek().type != Token::MapRange || left.hasError()) return left;

	std::vector<std::unique_ptr<TypeNode>> types;
	types.push_back(left.moveValue());
	while(peek().type == Token::MapRange) {
		Result<Token> andToken = advance();
		if (andToken.hasError()) {
			Token::Position pos = (current > 0) ? tokens[current-1].position : Token::Position{0, 0};
			return { andToken.getErrorMsg(), andToken.getErrorNote(), Trace(ASTNodeType::IntersectionType, pos) };
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> right = parseGenericType();
		if (right.hasError()) return {std::move(right), Trace(ASTNodeType::IntersectionType, pos)};

		types.push_back(right.moveValue());
	}

	return { std::make_unique<IntersectionTypeNode>(leftPos, std::move(types)) };
}

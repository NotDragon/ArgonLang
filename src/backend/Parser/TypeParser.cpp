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

	// Handle function types: func(i32, i32) i32 or func i32
	if(left.getValue().type == Token::KeywordFunc) {
		return parseFunctionType(left.getValue().position);
	}

	// Handle variadic types: ...i32
	if(left.getValue().type == Token::Ellipsis) {
		return parseVariadicType(left.getValue().position);
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

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseArrayType() {
	Result<std::unique_ptr<TypeNode>> base = parsePrefixedType();
	if(base.hasError()) return base;

	// Check for array type: i32[10]
	if(peek().type == Token::LeftBracket) {
		Token::Position pos = peek().position;
		Result<Token> leftBracket = advance();
		if (leftBracket.hasError()) {
			return { leftBracket.getErrorMsg(), leftBracket.getErrorNote(), Trace(ASTNodeType::ArrayType, pos) };
		}

		std::unique_ptr<ExpressionNode> size = nullptr;
		if(peek().type != Token::RightBracket) {
			// Parse array size expression
			auto sizeResult = parseExpression();
			if(sizeResult.hasError()) {
				return { sizeResult.getErrorMsg(), sizeResult.getErrorNote(), Trace(ASTNodeType::ArrayType, pos) };
			}
			// We need to cast to ExpressionNode since parseExpression returns ASTNode
			size = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(sizeResult.moveValue().release()));
		}

		Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']' after array size");
		if (rightBracket.hasError()) {
			return { rightBracket.getErrorMsg(), rightBracket.getErrorNote(), Trace(ASTNodeType::ArrayType, pos) };
		}

		return { std::make_unique<ArrayTypeNode>(pos, base.moveValue(), std::move(size)) };
	}

	// If not an array type, continue with generic type parsing
	if(peek().type != Token::Less) return base;

	return parseGenericTypeWithBase(base.moveValue());
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseGenericType() {
	return parseArrayType();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseGenericTypeWithBase(std::unique_ptr<TypeNode> base) {
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

	return { std::make_unique<GenericTypeNode>(base->position, std::move(base), std::move(args)) };
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

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseFunctionType(Token::Position pos) {
	// func(i32, i32) i32 or func i32 (closure)
	
	std::vector<std::unique_ptr<TypeNode>> paramTypes;
	std::unique_ptr<TypeNode> returnType = nullptr;
	bool isClosure = false;

	// Check if it's a closure (func i32) or regular function (func(i32, i32) i32)
	if(peek().type == Token::LeftParen) {
		// Regular function type: func(i32, i32) i32
		Result<Token> leftParen = advance();
		if (leftParen.hasError()) {
			return { leftParen.getErrorMsg(), leftParen.getErrorNote(), Trace(ASTNodeType::FunctionType, pos) };
		}

		// Parse parameter types
		while(peek().type != Token::RightParen) {
			auto paramType = parseType();
			if(paramType.hasError()) {
				return { paramType.getErrorMsg(), paramType.getErrorNote(), Trace(ASTNodeType::FunctionType, pos) };
			}
			paramTypes.push_back(paramType.moveValue());

			if(peek().type == Token::RightParen) break;

			Result<Token> comma = expect(Token::Comma, "Expected ',' between parameter types");
			if (comma.hasError()) {
				return { comma.getErrorMsg(), comma.getErrorNote(), Trace(ASTNodeType::FunctionType, pos) };
			}
		}

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after parameter types");
		if (rightParen.hasError()) {
			return { rightParen.getErrorMsg(), rightParen.getErrorNote(), Trace(ASTNodeType::FunctionType, pos) };
		}

		// Parse optional return type
		if(peek().type != Token::Semicolon && peek().type != Token::Comma && 
		   peek().type != Token::RightParen && peek().type != Token::RightBrace && 
		   peek().type != Token::FilterRange && peek().type != Token::MapRange) {
			auto retType = parseType();
			if(retType.hasError()) {
				return { retType.getErrorMsg(), retType.getErrorNote(), Trace(ASTNodeType::FunctionType, pos) };
			}
			returnType = retType.moveValue();
		}
	} else {
		// Closure type: func i32
		isClosure = true;
		auto retType = parseType();
		if(retType.hasError()) {
			return { retType.getErrorMsg(), retType.getErrorNote(), Trace(ASTNodeType::FunctionType, pos) };
		}
		returnType = retType.moveValue();
	}

	return { std::make_unique<FunctionTypeNode>(pos, std::move(paramTypes), std::move(returnType), isClosure) };
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parseVariadicType(Token::Position pos) {
	// ...i32
	auto baseType = parseType();
	if(baseType.hasError()) {
		return { baseType.getErrorMsg(), baseType.getErrorNote(), Trace(ASTNodeType::VariadicType, pos) };
	}

	return { std::make_unique<VariadicTypeNode>(pos, baseType.moveValue()) };
}

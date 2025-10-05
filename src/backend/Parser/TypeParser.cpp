#include "backend/Parser.h"
#include "Error/Error.h"
#include "Error/Result.h"

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_type() {
	return parse_sum_type();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_identifier_type() {
	Result<Token> left = advance();
	if (!left.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(left.error());
	}

	if (left.value().type == Token::LeftParen) {
		Result<std::unique_ptr<TypeNode>> type = parse_type();
		Result<Token> token_error1 = expect(Token::RightParen, "Expected closing ')'");
		if (!tokenError1.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(tokenError1.error());
		}
		return type;
	}

	// Handle function types: func(i32, i32) i32 or func i32
	if (left.value().type == Token::KeywordFunc) {
		return parse_function_type(left.value().position);
	}

	// Handle variadic types: ...i32
	if (left.value().type == Token::Ellipsis) {
		return parse_variadic_type(left.value().position);
	}

	if (left.value().type != Token::Identifier && left.value().type != Token::PrimitiveType) {
		return Err<std::unique_ptr<TypeNode>>(
		    create_parse_error(ErrorType::UnexpectedToken, "Expected type", left.value().position));
	}

	if (peek().type == Token::Identifier || peek().type == Token::PrimitiveType) {
		return Err<std::unique_ptr<TypeNode>>(
		    create_parse_error(ErrorType::UnexpectedToken, "Expected '&' or '|' between types", left.value().position));
	}

	return Ok(std::make_unique<IdentifierTypeNode>(left.value().position, left.value().value));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_prefixed_type() {
	PrefixedTypeNode::Prefix prefix;
	switch (peek().type) {
	case Token::Multiply:
		prefix = PrefixedTypeNode::Prefix::Pointer;
		break;
	case Token::Ownership:
		prefix = PrefixedTypeNode::Prefix::Owned;
		break;
	default:
		return parse_identifier_type();
	}

	Result<Token> pref = advance();
	if (!pref.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(pref.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> base = parse_identifier_type();
	if (!base.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(base.error());
	}

	return Ok(std::make_unique<PrefixedTypeNode>(base.value()->position, std::move(base.value()), prefix));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_array_type() {
	Result<std::unique_ptr<TypeNode>> base = parse_prefixed_type();
	if (!base.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(base.error());
	}

	// Check for array type: i32[10]
	if (peek().type == Token::LeftBracket) {
		Token::Position pos = peek().position;
		Result<Token> left_bracket = advance();
		if (!leftBracket.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(leftBracket.error());
		}

		std::unique_ptr<ExpressionNode> size = nullptr;
		if (peek().type != Token::RightBracket) {
			// Parse array size expression
			auto size_result = parse_expression();
			if (!size_result.has_value()) {
				return Err<std::unique_ptr<TypeNode>>(size_result.error());
			}
			// We need to cast to ExpressionNode since parseExpression returns ASTNode
			size = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(size_result.value().release()));
		}

		Result<Token> right_bracket = expect(Token::RightBracket, "Expected ']' after array size");
		if (!rightBracket.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(rightBracket.error());
		}

		return Ok(std::make_unique<ArrayTypeNode>(pos, std::move(base.value()), std::move(size)));
	}

	// If not an array type, continue with generic type parsing
	if (peek().type != Token::Less)
		return base;

	return parse_generic_type_with_base(std::move(base.value()));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_generic_type() {
	return parse_array_type();
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>>
ArgonLang::Parser::parse_generic_type_with_base(std::unique_ptr<TypeNode> base) {
	Token::Position pos = peek().position;
	Result<Token> less = advance();
	if (!less.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(less.error());
	}

	std::vector<std::unique_ptr<TypeNode>> args;
	while (peek().type != Token::Greater) {
		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> arg = parse_type();
		if (!arg.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(arg.error());
		}

		args.push_back(std::move(arg.value()));

		if (peek().type == Token::Greater)
			break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' or '>'");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(comma.error());
		}
	}

	Result<Token> greater = advance();
	if (!greater.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(greater.error());
	}

	return Ok(std::make_unique<GenericTypeNode>(base->position, std::move(base), std::move(args)));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_sum_type() {
	Token::Position left_pos = peek().position;
	Result<std::unique_ptr<TypeNode>> left = parse_intersection_type();
	if (!left.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(left.error());
	}
	if (peek().type != Token::FilterRange)
		return left;

	std::vector<std::unique_ptr<TypeNode>> types;
	types.push_back(std::move(left.value()));
	while (peek().type == Token::FilterRange) {
		Result<Token> or_token = advance();
		if (!orToken.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(orToken.error());
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> right = parse_intersection_type();
		if (!right.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(right.error());
		}

		types.push_back(std::move(right.value()));
	}

	return Ok(std::make_unique<SumTypeNode>(left_pos, std::move(types)));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_intersection_type() {
	Token::Position left_pos = peek().position;
	Result<std::unique_ptr<TypeNode>> left = parse_generic_type();
	if (!left.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(left.error());
	}
	if (peek().type != Token::MapRange)
		return left;

	std::vector<std::unique_ptr<TypeNode>> types;
	types.push_back(std::move(left.value()));
	while (peek().type == Token::MapRange) {
		Result<Token> and_token = advance();
		if (!andToken.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(andToken.error());
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<TypeNode>> right = parse_generic_type();
		if (!right.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(right.error());
		}

		types.push_back(std::move(right.value()));
	}

	return Ok(std::make_unique<IntersectionTypeNode>(left_pos, std::move(types)));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_function_type(Token::Position pos) {
	// func(i32, i32) i32 or func i32 (closure)

	std::vector<std::unique_ptr<TypeNode>> param_types;
	std::unique_ptr<TypeNode> return_type = nullptr;
	bool is_closure = false;

	// Check if it's a closure (func i32) or regular function (func(i32, i32) i32)
	if (peek().type == Token::LeftParen) {
		// Regular function type: func(i32, i32) i32
		Result<Token> left_paren = advance();
		if (!leftParen.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(leftParen.error());
		}

		// Parse parameter types
		while (peek().type != Token::RightParen) {
			auto param_type = parse_type();
			if (!param_type.has_value()) {
				return Err<std::unique_ptr<TypeNode>>(param_type.error());
			}
			param_types.push_back(std::move(param_type.value()));

			if (peek().type == Token::RightParen)
				break;

			Result<Token> comma = expect(Token::Comma, "Expected ',' between parameter types");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<TypeNode>>(comma.error());
			}
		}

		Result<Token> right_paren = expect(Token::RightParen, "Expected ')' after parameter types");
		if (!rightParen.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(rightParen.error());
		}

		// Parse optional return type
		if (peek().type != Token::Semicolon && peek().type != Token::Comma && peek().type != Token::RightParen &&
		    peek().type != Token::RightBrace && peek().type != Token::FilterRange && peek().type != Token::MapRange) {
			auto ret_type = parse_type();
			if (!ret_type.has_value()) {
				return Err<std::unique_ptr<TypeNode>>(ret_type.error());
			}
			return_type = std::move(ret_type.value());
		}
	} else {
		// Closure type: func i32
		is_closure = true;
		auto ret_type = parse_type();
		if (!ret_type.has_value()) {
			return Err<std::unique_ptr<TypeNode>>(ret_type.error());
		}
		return_type = std::move(ret_type.value());
	}

	return Ok(std::make_unique<FunctionTypeNode>(pos, std::move(param_types), std::move(return_type), is_closure));
}

ArgonLang::Result<std::unique_ptr<ArgonLang::TypeNode>> ArgonLang::Parser::parse_variadic_type(Token::Position pos) {
	// ...i32
	auto base_type = parse_type();
	if (!base_type.has_value()) {
		return Err<std::unique_ptr<TypeNode>>(base_type.error());
	}

	return Ok(std::make_unique<VariadicTypeNode>(pos, std::move(base_type.value())));
}

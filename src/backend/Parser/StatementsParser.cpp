#include "backend/Parser.h"
#include "Error/Error.h"
#include "Error/Result.h"

#include <stdexcept>

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parse_statement() {
	switch (peek().type) {
	case Token::KeywordUnion:
		return parse_union_declaration();
		//		case Token::KeywordEval:
		//			break;
		//		case Token::KeywordType:
		//			break;
	case Token::KeywordDef:
		return parse_variable_declaration();
	case Token::KeywordIf:
		return parse_if_statement();
	case Token::KeywordWhile:
		return parse_while_statement();
	case Token::KeywordFor:
		return parse_for_statement();
	case Token::KeywordReturn:
		return parse_return_statement();
	case Token::KeywordFunc:
		return parse_function_declaration();
	case Token::KeywordClass:
		return parse_class_declaration();
	case Token::KeywordConstructor:
		return parse_constructor_statement();
	case Token::KeywordImpl:
		return parse_impl_statement();
	case Token::KeywordUsing:
		return parse_type_alias();
	case Token::KeywordYield:
		return parse_yield_statement();
	case Token::KeywordEnum:
		return parse_enum_declaration();
	case Token::KeywordConstraint:
		return parse_constraint_declaration();
	case Token::KeywordModule:
		return parse_module_declaration();
	case Token::KeywordImport:
		return parse_import_statement();
	case Token::KeywordPar:
		return parse_parallel_expression();
	case Token::KeywordBreak:
		return parse_break_statement();
	case Token::KeywordContinue:
		return parse_continue_statement();
	case Token::LeftBrace:
		return parse_block();
	default:
		return parse_expression();
	}
}

Result<std::unique_ptr<ASTNode>> Parser::parse_variable_declaration() {
	Result<Token> keyword_result = advance();
	if (!keyword_result.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keyword_result.error());
	}
	Token keyword = keyword_result.value();

	// Parse the left-hand side (can be simple name, single pattern, or compound patterns)
	bool is_simple = false;
	bool is_single_pattern = false;
	bool is_compound_pattern = false;
	std::string name = "";
	std::unique_ptr<PatternNode> pattern = nullptr;
	std::vector<std::unique_ptr<PatternNode>> compound_patterns;

	// Parse first element
	if (peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
		// Starts with a pattern
		Result<std::unique_ptr<PatternNode>> first_pattern = parse_pattern();
		if (!first_pattern.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(first_pattern.error());
		}

		// Check if there's a comma (compound pattern)
		if (peek().type == Token::Comma) {
			// This is compound destructuring: [first], rest = ...
			is_compound_pattern = true;
			compound_patterns.push_back(std::move(first_pattern.value()));

			// Parse remaining patterns
			while (peek().type == Token::Comma) {
				advance(); // consume comma

				if (peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
					// Another pattern
					Result<std::unique_ptr<PatternNode>> next_pattern = parse_pattern();
					if (!next_pattern.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(next_pattern.error());
					}
					compound_patterns.push_back(std::move(next_pattern.value()));
				} else if (peek().type == Token::Identifier) {
					// Rest identifier
					Result<Token> rest_id = advance();
					if (!rest_id.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(rest_id.error());
					}
					compound_patterns.push_back(
					    std::make_unique<IdentifierPatternNode>(rest_id.value().position, rest_id.value().value));
				} else {
					Token current_token = peek();
					Error error = create_parse_error(
					    ErrorType::UnexpectedToken,
					    "Expected pattern or identifier after comma in compound destructuring", current_token.position);
					error.withExpected("pattern or identifier").withActual(current_token.value);
					return Err<std::unique_ptr<ASTNode>>(error);
				}
			}
		} else {
			// Single pattern destructuring: def [a, b] = ...
			is_single_pattern = true;
			pattern = std::move(first_pattern.value());
		}
	} else if (peek().type == Token::Identifier) {
		// Could be simple declaration or compound starting with identifier
		Result<Token> first_id = advance();
		if (!first_id.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(first_id.error());
		}

		if (peek().type == Token::Comma) {
			// Compound destructuring starting with identifier: def rest, [last] = ...
			is_compound_pattern = true;
			compound_patterns.push_back(
			    std::make_unique<IdentifierPatternNode>(first_id.value().position, first_id.value().value));

			// Parse remaining patterns
			while (peek().type == Token::Comma) {
				advance(); // consume comma

				if (peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
					Result<std::unique_ptr<PatternNode>> next_pattern = parse_pattern();
					if (!next_pattern.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(next_pattern.error());
					}
					compound_patterns.push_back(std::move(next_pattern.value()));
				} else if (peek().type == Token::Identifier) {
					Result<Token> rest_id = advance();
					if (!rest_id.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(rest_id.error());
					}
					compound_patterns.push_back(
					    std::make_unique<IdentifierPatternNode>(rest_id.value().position, rest_id.value().value));
				} else {
					return Err<std::unique_ptr<ASTNode>>(create_parse_error(
					    ErrorType::UnexpectedToken,
					    "Expected pattern or identifier after comma in compound destructuring", peek().position));
				}
			}
		} else {
			// Simple declaration: def name = ...
			is_simple = true;
			name = first_id.value().value;
		}
	} else {
		return Err<std::unique_ptr<ASTNode>>(create_parse_error(
		    ErrorType::UnexpectedToken, "Expected identifier or pattern after 'def'", keyword.position));
	}

	Result<std::unique_ptr<TypeNode>> type;
	Result<std::unique_ptr<ASTNode>> value;

	if (peek().type == Token::Colon) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token.error());
		}

		type = parse_type();
		if (!type.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(type.error());
		}
	}

	if (peek().type == Token::Assign) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token.error());
		}

		value = parse_expression();
		if (!value.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(value.error());
		}
	}
	Token::Position smiColonPos = peek().position;
	Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';'");
	if (!semi_colon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
	}

	if (!type.has_value() && !value.has_value()) {
		//		return { "Expected either value or type, in variable declaration" };
	}

	if (is_compound_pattern) {
		return Ok(std::make_unique<VariableDeclarationNode>(
		    keyword.position, (keyword.value == "const"), type.value() ? std::move(type.value()) : nullptr,
		    value.value() ? dynamic_unique_cast<ExpressionNode>(std::move(value.value())) : nullptr,
		    std::move(compound_patterns)));
	} else if (is_single_pattern) {
		return Ok(std::make_unique<VariableDeclarationNode>(
		    keyword.position, (keyword.value == "const"), type.value() ? std::move(type.value()) : nullptr,
		    value.value() ? dynamic_unique_cast<ExpressionNode>(std::move(value.value())) : nullptr,
		    std::move(pattern)));
	} else {
		return Ok(std::make_unique<VariableDeclarationNode>(
		    keyword.position, (keyword.value == "const"), type.value() ? std::move(type.value()) : nullptr,
		    value.value() ? dynamic_unique_cast<ExpressionNode>(std::move(value.value())) : nullptr, name));
	}
}

Result<std::unique_ptr<FunctionArgument>> Parser::parse_function_argument() {
	Token::Position start_pos = peek().position;

	Result<Token> arg_identifier = expect(Token::Identifier, "Expected identifier in function argument");
	if (!arg_identifier.has_value()) {
		return Err<std::unique_ptr<FunctionArgument>>(arg_identifier.error());
	}

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;

	if (peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(colon.error());
		}

		Result<std::unique_ptr<TypeNode>> type_res = parse_type();
		if (!type_res.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(type_res.error());
		}

		type = std::move(type_res.value());
	}

	if (peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (!assign.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(assign.error());
		}

		Result<std::unique_ptr<ASTNode>> value_res = parse_expression();
		if (!value_res.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(value_res.error());
		}

		value = dynamic_unique_cast<ExpressionNode>(std::move(value_res.value()));
	}

	return Ok(
	    std::make_unique<FunctionArgument>(start_pos, std::move(type), std::move(value), arg_identifier.value().value));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_function_declaration() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	// Parse generic parameters if present: func<T, U>
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if (peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if (!less.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(less.error());
		}

		while (peek().type != Token::Greater) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parse_generic_parameter();
			if (!param.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(param.error());
			}

			genericParams.push_back(std::move(param.value()));

			if (peek().type == Token::Greater)
				break;
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between generic parameters");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if (!greater.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(greater.error());
		}
	}

	Result<std::unique_ptr<ASTNode>> identifier = parse_member_access_expression();
	if (!identifier.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(identifier.error());
	}

	if (identifier.has_value() && identifier.value()->get_node_type() == ASTNodeType::Identifier) {
		auto tempIdentifier = dynamic_cast<IdentifierNode*>(identifier.value().get());
		if (tempIdentifier->identifier == "main")
			main_counter++;
	}

	Token::Position left_paren_pos = peek().position;
	Result<Token> left_paren = expect(Token::LeftParen, "Expected '(' after function declaration");
	if (!left_paren.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left_paren.error());
	}

	std::vector<std::unique_ptr<FunctionArgument>> args;
	while (peek().type != Token::RightParen) {
		Token::Position arg_pos = peek().position;
		Result<std::unique_ptr<FunctionArgument>> arg = parse_function_argument();
		if (!arg.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arg.error());
		}

		args.push_back(std::move(arg.value()));

		if (peek().type == Token::RightParen)
			break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> right_paren = expect(Token::RightParen, "Expected ')' after function declaration");
	if (!right_paren.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(right_paren.error());
	}

	Result<std::unique_ptr<TypeNode>> return_type;

	// Handle function syntax patterns:
	// 1. func name(args) ReturnType -> expression;  (inline expression with return type)
	// 2. func name(args) -> expression;             (inline expression without return type)
	// 3. func name(args) ReturnType { body }        (function with return type and body)
	// 4. func name(args) { body }                   (function without return type)
	// 5. func name(args);                           (declaration only)

	// First, check if we have a return type (not immediately -> or { or ;)
	if (peek().type != Token::Arrow && peek().type != Token::LeftBrace && peek().type != Token::Semicolon) {
		// Parse return type: func name(args) ReturnType ...
		Token::Position type_pos = peek().position;

		Result<std::unique_ptr<TypeNode>> type_result = parse_type();
		if (type_result.has_value()) {
			return_type = std::move(type_result);
		}
		// If type parsing failed, we'll continue and let the arrow/body/semicolon parsing handle it
	}

	// Now check for arrow (inline expression)
	if (peek().type == Token::Arrow) {
		// Arrow syntax for inline expression: -> expression;
		Result<Token> arrow = advance();
		if (!arrow.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arrow.error());
		}

		Token::Position expr_pos = peek().position;

		// Parse the inline expression
		Result<std::unique_ptr<ASTNode>> expr = parse_expression();
		if (!expr.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(expr.error());
		}

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(
		    expr_pos, dynamic_unique_cast<ExpressionNode>(std::move(expr.value())), false);

		Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';' after inline function");
		if (!semi_colon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
		}

		return Ok(std::make_unique<FunctionDeclarationNode>(
		    token.value().position, std::move(return_type.value()), std::move(args), std::move(body),
		    dynamic_unique_cast<ExpressionNode>(std::move(identifier.value())), std::move(genericParams)));
	}

	if (peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if (!semicolon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(semicolon.error());
		}
		return Ok(std::make_unique<FunctionDefinitionNode>(
		    token.value().position, std::move(return_type.value()), std::move(args),
		    dynamic_unique_cast<ExpressionNode>(std::move(identifier.value())), std::move(genericParams)));
	}

	Result<std::unique_ptr<ASTNode>> body = parse_statement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<FunctionDeclarationNode>(
	    token.value().position, return_type.value() ? std::move(return_type.value()) : nullptr, std::move(args),
	    std::move(body.value()), dynamic_unique_cast<ExpressionNode>(std::move(identifier.value())),
	    std::move(genericParams)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_if_statement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> left_paren = expect(Token::LeftParen, "Expected '(' after if statement");
	if (!left_paren.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left_paren.error());
	}

	Token::Position condition_pos = peek().position;
	Result<std::unique_ptr<ASTNode>> condition = parse_expression();
	if (!condition.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(condition.error());
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	Token::Position body_pos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parse_statement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	Result<std::unique_ptr<ASTNode>> else_statement;

	if (peek().type == Token::KeywordElse) {
		Result<Token> token1 = advance();
		if (!token1.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token1.error());
		}

		else_statement = parse_statement();
		if (!else_statement.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(else_statement.error());
		}
	}
	return Ok(std::make_unique<IfStatementNode>(
	    token.value().position, dynamic_unique_cast<ExpressionNode>(std::move(condition.value())),
	    dynamic_unique_cast<StatementNode>(std::move(body.value())),
	    else_statement.value() ? dynamic_unique_cast<StatementNode>(std::move(else_statement.value())) : nullptr));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_for_statement() {
	Result<Token> keyword = advance();
	if (!keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keyword.error());
	}
	Result<Token> left_paren = expect(Token::LeftParen, "Expected '(' after for statement");
	if (!left_paren.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left_paren.error());
	}

	Result<Token> identifier_res = expect(Token::Identifier, "Expected identifier");
	if (!identifier_res.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(identifier_res.error());
	}

	std::unique_ptr<TypeNode> type;
	if (peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(colon.error());
		}

		Result<std::unique_ptr<TypeNode>> typeRes = parse_type();
		if (!typeRes.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(typeRes.error());
		}
		type = std::move(typeRes.value());
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->' after variable");
	if (!arrow.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(arrow.error());
	}

	Result<std::unique_ptr<ASTNode>> iterator = parse_expression();
	if (!iterator.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(iterator.error());
	}

	Result<Token> right_paren = expect(Token::RightParen, "Expected ')' after expression");
	if (!right_paren.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(right_paren.error());
	}

	Result<std::unique_ptr<ASTNode>> body = parse_statement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<ForStatementNode>(keyword.value().position, identifier_res.value().value,
	                                             dynamic_unique_cast<ExpressionNode>(std::move(iterator.value())),
	                                             dynamic_unique_cast<StatementNode>(std::move(body.value())),
	                                             std::move(type)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_while_statement() {
	Result<Token> keyword_res = advance();
	if (!keyword_res.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keyword_res.error());
	}
	Token keyword = keyword_res.value();

	Result<Token> left_paren = expect(Token::LeftParen, "Expected '(' after while statement");
	if (!left_paren.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left_paren.error());
	}

	Token::Position condition_pos = peek().position;
	Result<std::unique_ptr<ASTNode>> condition = parse_expression();
	if (!condition.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(condition.error());
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	Token::Position body_pos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parse_statement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	Result<std::unique_ptr<ASTNode>> else_statement;

	if (peek().type == Token::KeywordElse) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token.error());
		}

		else_statement = parse_statement();
		if (!else_statement.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(else_statement.error());
		}
	}
	return Ok(std::make_unique<WhileStatementNode>(
	    keyword.position, keyword.value == "dowhile", dynamic_unique_cast<ExpressionNode>(std::move(condition.value())),
	    dynamic_unique_cast<StatementNode>(std::move(body.value())),
	    else_statement.value() ? dynamic_unique_cast<StatementNode>(std::move(else_statement.value())) : nullptr));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_return_statement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	bool isSuper = false;
	if (peek().type == Token::KeywordSuper) {
		isSuper = true;
		Result<Token> super = advance();
		if (!super.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(super.error());
		}
	}

	if (peek().type == Token::Semicolon) {
		advance();
		return Ok(std::make_unique<ReturnStatementNode>(
			token.value().position, nullptr, isSuper));
	}

	Result<std::unique_ptr<ASTNode>> expr = parse_expression();
	if (!expr.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(expr.error());
	}

	Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';'");
	if (!semi_colon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
	}

	return Ok(std::make_unique<ReturnStatementNode>(
	    token.value().position, dynamic_unique_cast<ExpressionNode>(std::move(expr.value())), isSuper));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_break_statement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';'");
	if (!semi_colon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
	}

	return Ok(std::make_unique<BreakStatementNode>(token.value().position));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_continue_statement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';'");
	if (!semi_colon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
	}

	return Ok(std::make_unique<ContinueStatementNode>(token.value().position));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_type_alias() {
	Result<Token> type_alias = advance();
	if (!type_alias.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(type_alias.error());
	}

	Result<Token> identifier = expect(Token::Identifier, "Expected identifier after using");
	if (!identifier.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(identifier.error());
	}

	Result<Token> assign = expect(Token::Assign, "Expected '=' after using");
	if (!assign.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(assign.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> type = parse_type();
	if (!type.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(type.error());
	}

	Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';' after type alias");
	if (!semi_colon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
	}

	return Ok(std::make_unique<TypeAliasNode>(type_alias.value().position, identifier.value().value,
	                                          std::move(type.value())));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_class_declaration() {
	Result<Token> class_keyword = advance();
	if (!class_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(class_keyword.error());
	}

	Result<Token> class_name = advance();
	if (!class_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(class_name.error());
	}

	current_class_name = class_name.value().value;

	// Check for default visibility modifier after class name: class Name pub { ... }
	MemberVisibility defaultVisibility = MemberVisibility::PRI;
	if (peek().type == Token::KeywordPub) {
		Result<Token> pub_keyword = advance();
		if (!pub_keyword.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(pub_keyword.error());
		}
		defaultVisibility = MemberVisibility::PUB;
	}

	// Parse generic parameters if present: class Name<T, U> { ... }
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if (peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if (!less.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(less.error());
		}

		// Parse generic parameters
		do {
			Token::Position param_pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parse_generic_parameter();
			if (!param.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(param.error());
			}

			genericParams.push_back(std::move(param.value()));

			if (peek().type == Token::Comma) {
				Token::Position commaPos = peek().position;
				Result<Token> comma = advance();
				if (!comma.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(comma.error());
				}
			} else {
				break;
			}
		} while (true);

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if (!greater.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(greater.error());
		}
	}

	// Parse base classes if present: class Name impl Base1, Base2 { ... }
	std::vector<std::unique_ptr<TypeNode>> baseClasses;
	if (peek().type == Token::KeywordImpl) {
		Result<Token> impl_keyword = advance();
		if (!impl_keyword.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(impl_keyword.error());
		}

		// Parse base class list
		do {
			Result<std::unique_ptr<TypeNode>> base_type = parse_type();
			if (!base_type.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(base_type.error());
			}

			baseClasses.push_back(std::move(base_type.value()));

			if (peek().type == Token::Comma) {
				Result<Token> comma = advance();
				if (!comma.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(comma.error());
				}
			} else {
				break;
			}
		} while (true);
	}

	std::vector<ConstructorStatementNode> constructors;
	std::vector<ClassDeclarationNode::ClassMember> members;

	//  TODO: in the very far future
	//	if(peek().type == Token::RightParen) {
	//
	//	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after class declaration");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	while (peek().type != Token::RightBrace) {
		MemberVisibility visibility = defaultVisibility;

		// Check for explicit visibility modifier
		if (peek().type == Token::KeywordPub || peek().type == Token::KeywordPri || peek().type == Token::KeywordPro) {
			Result<Token> vis_keyword = advance();
			if (!vis_keyword.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(vis_keyword.error());
			}

			visibility = vis_keyword.value().type == Token::KeywordPub   ? MemberVisibility::PUB
			             : vis_keyword.value().type == Token::KeywordPri ? MemberVisibility::PRI
			                                                             : MemberVisibility::PRO;
		}

		// Check for const/mut modifiers for bare field declarations
		bool isConst = false;
		bool isMut = false;
		std::string constOrDefValue;

		// Check if we have a const (which maps to KeywordDef with value "const")
		if (peek().type == Token::KeywordDef && peek().value == "const") {
			isConst = true;
			constOrDefValue = peek().value;
			Result<Token> const_keyword = advance();
			if (!const_keyword.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(const_keyword.error());
			}
		} else if (peek().type == Token::KeywordMut) {
			isMut = true;
			Result<Token> mut_keyword = advance();
			if (!mut_keyword.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(mut_keyword.error());
			}
		}

		Token::Position memberPos = peek().position;
		Result<std::unique_ptr<ASTNode>> member;

		// Check what kind of member we're parsing
		if (peek().type == Token::KeywordDef) {
			member = parse_variable_declaration();
		} else if (peek().type == Token::KeywordFunc) {
			member = parse_function_declaration();
		} else if (peek().type == Token::KeywordConstructor) {
			member = parse_constructor_statement();
		} else if (peek().type == Token::Identifier) {
			// Bare field declaration: fieldName: Type; or fieldName: Type = value;
			Token::Position fieldPos = peek().position;
			Result<Token> fieldName = advance();
			if (!fieldName.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(fieldName.error());
			}

			Result<Token> colon = expect(Token::Colon, "Expected ':' after field name");
			if (!colon.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(colon.error());
			}

			Result<std::unique_ptr<TypeNode>> fieldType = parse_type();
			if (!fieldType.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(fieldType.error());
			}

			std::unique_ptr<ExpressionNode> fieldValue = nullptr;
			if (peek().type == Token::Assign) {
				Result<Token> assign = advance();
				if (!assign.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(assign.error());
				}

				Result<std::unique_ptr<ASTNode>> value = parse_expression();
				if (!value.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(value.error());
				}
				fieldValue = dynamic_unique_cast<ExpressionNode>(std::move(value.value()));
			}

			Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after field declaration");
			if (!semicolon.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(semicolon.error());
			}

			// Create a VariableDeclarationNode for the field
			member = Ok(std::make_unique<VariableDeclarationNode>(
			    fieldPos, isConst, std::move(fieldType.value()), std::move(fieldValue), fieldName.value().value));
		} else {
			return Err<std::unique_ptr<ASTNode>>(create_parse_error(
			    ErrorType::UnexpectedToken,
			    "Expected field declaration, function declaration, or constructor declaration",
			    peek().position));
		}

		if (!member.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(member.error());
		}
		Token::Position memberPosition = member.value()->position;

		// Check if the parsed member is actually a StatementNode
		if (member.value()->get_node_group() != ASTNodeGroup::Statement) {
			return Err<std::unique_ptr<ASTNode>>(create_parse_error(
			    ErrorType::UnexpectedToken,
			    "Expected statement in class body, got " + ast_node_type_to_string(member.value()->get_node_type()),
			    memberPosition));
		}

		members.emplace_back(memberPosition, dynamic_unique_cast<StatementNode>(std::move(member.value())), visibility);
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after class declaration");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<ClassDeclarationNode>(class_keyword.value().position, class_name.value().value,
	                                                 std::move(members), std::move(genericParams), std::move(baseClasses)));
}

Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> Parser::parse_constructor_argument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if (!argIdentifier.has_value()) {
		return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(argIdentifier.error());
	}

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;
	std::string initializes;

	if (peek().type == Token::LeftParen) {
		Result<Token> leftParen = expect(Token::LeftParen, "Expected '('");
		if (!leftParen.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(leftParen.error());
		}

		Result<Token> initIdentifier = expect(Token::Identifier, "Expected identifier");
		if (!initIdentifier.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(initIdentifier.error());
		}
		initializes = initIdentifier.value().value;

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if (!rightParen.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(rightParen.error());
		}
	}

	if (peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(colon.error());
		}

		Result<std::unique_ptr<TypeNode>> typeRes = parse_type();
		if (!typeRes.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(typeRes.error());
		}

		type = std::move(typeRes.value());
	}

	if (peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (!assign.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(assign.error());
		}

		Result<std::unique_ptr<ASTNode>> valueRes = parse_expression();
		if (!valueRes.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(valueRes.error());
		}

		value = dynamic_unique_cast<ExpressionNode>(std::move(valueRes.value()));
	}

	return Ok(std::make_unique<ConstructorStatementNode::ConstructorArgument>(
	    argIdentifier.value().position, argIdentifier.value().value, initializes, std::move(type), std::move(value)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_constructor_statement() {
	Result<Token> constructor_keyword = advance();
	if (!constructor_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constructor_keyword.error());
	}

	std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after constructor");
	if (!leftParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftParen.error());
	}

	while (true) {
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> arg = parse_constructor_argument();
		if (!arg.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arg.error());
		}

		args.push_back(std::move(arg.value()));

		if (peek().type == Token::RightParen)
			break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after constructor");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	if (peek().type == Token::Semicolon) {
		return Ok(
		    std::make_unique<ConstructorStatementNode>(current_class_name, constructor_keyword.value().position, std::move(args), nullptr));
	}

	Result<std::unique_ptr<ASTNode>> body = parse_statement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<ConstructorStatementNode>(current_class_name, constructor_keyword.value().position, std::move(args),
	                                                     std::move(body.value())));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_block() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	std::vector<std::unique_ptr<ASTNode>> body;
	while (peek().type != Token::RightBrace) {
		Result<std::unique_ptr<ASTNode>> statement = parse_statement();
		if (!statement.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(statement.error());
		}

		if (statement.value()->get_node_group() == ASTNodeGroup::Expression) {
			Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';'");
			if (!semi_colon.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
			}
		}

		body.push_back(std::move(statement.value()));
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after opening '{'");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<BlockNode>(token.value().position, std::move(body)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_impl_statement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> class_name = expect(Token::Identifier, "Expected Class Name");
	if (!class_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(class_name.error());
	}

	Result<Token> keyword = advance();
	if (!keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keyword.error());
	}

	MemberVisibility visibility = MemberVisibility::PRI;
	if (keyword.value().type == Token::KeywordPub || keyword.value().type == Token::KeywordPri ||
	    keyword.value().type == Token::KeywordPro) {
		visibility = keyword.value().type == Token::KeywordPub   ? MemberVisibility::PUB
		             : keyword.value().type == Token::KeywordPri ? MemberVisibility::PRI
		                                                         : MemberVisibility::PRO;

		keyword = advance();
		if (!keyword.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(keyword.error());
		}
	}

	if (keyword.value().type != Token::KeywordFunc && keyword.value().type != Token::KeywordDef) {
		return Err<std::unique_ptr<ASTNode>>(create_parse_error(
		    ErrorType::UnexpectedToken, "Expected variable or function declaration", token.value().position));
	}

	Token::Position body_pos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parse_statement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<ImplStatementNode>(token.value().position, class_name.value().value,
	                                              dynamic_unique_cast<StatementNode>(std::move(body.value())),
	                                              visibility));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_yield_statement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<std::unique_ptr<ASTNode>> expr = parse_expression();
	if (!expr.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(expr.error());
	}

	Result<Token> semi_colon = expect(Token::Semicolon, "Expected ';'");
	if (!semi_colon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semi_colon.error());
	}

	return Ok(std::make_unique<YieldStatementNode>(token.value().position,
	                                               dynamic_unique_cast<ExpressionNode>(std::move(expr.value()))));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_enum_declaration() {
	Result<Token> enum_keyword = advance();
	if (!enum_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(enum_keyword.error());
	}

	bool is_union = false;
	if (peek().type == Token::KeywordUnion) {
		is_union = true;
		Result<Token> union_keyword = advance();
		if (!union_keyword.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(union_keyword.error());
		}
	}

	Result<Token> enum_name = expect(Token::Identifier, "Expected enum name");
	if (!enum_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(enum_name.error());
	}

	// Parse optional constraint: enum Name -> ConstraintType { ... }
	std::unique_ptr<TypeNode> constraintType = nullptr;
	if (peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if (!arrow.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arrow.error());
		}

		Result<std::unique_ptr<TypeNode>> constraint = parse_type();
		if (!constraint.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(constraint.error());
		}
		constraintType = std::move(constraint.value());
	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after enum name");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	std::vector<EnumDeclarationNode::EnumVariant> variants;
	while (peek().type != Token::RightBrace) {
		Result<Token> variantName = expect(Token::Identifier, "Expected variant name");
		if (!variantName.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(variantName.error());
		}

		std::vector<EnumDeclarationNode::EnumField> fields;
		std::unique_ptr<ExpressionNode> explicitValue = nullptr;

		// Check for explicit value: VariantName = value
		if (peek().type == Token::Assign) {
			Result<Token> assign = advance();
			if (!assign.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(assign.error());
			}

			Result<std::unique_ptr<ASTNode>> value = parse_expression();
			if (!value.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(value.error());
			}
			explicitValue = dynamic_unique_cast<ExpressionNode>(std::move(value.value()));
		}
		// Check for structured fields with curly braces: VariantName{ field: Type, ... }
		else if (peek().type == Token::LeftBrace) {
			Result<Token> leftBraceFields = advance();
			if (!leftBraceFields.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(leftBraceFields.error());
			}

			while (peek().type != Token::RightBrace) {
				Token::Position fieldPos = peek().position;
				Result<Token> fieldName = expect(Token::Identifier, "Expected field name");
				if (!fieldName.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(fieldName.error());
				}

				Result<Token> colon = expect(Token::Colon, "Expected ':' after field name");
				if (!colon.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(colon.error());
				}

				Result<std::unique_ptr<TypeNode>> fieldType = parse_type();
				if (!fieldType.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(fieldType.error());
				}

				fields.emplace_back(fieldPos, fieldName.value().value, std::move(fieldType.value()));

				if (peek().type == Token::RightBrace)
					break;
				Result<Token> comma = expect(Token::Comma, "Expected ',' between enum fields");
				if (!comma.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(comma.error());
				}
			}

			Result<Token> rightBraceFields = expect(Token::RightBrace, "Expected '}' after enum fields");
			if (!rightBraceFields.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(rightBraceFields.error());
			}
		}

		variants.emplace_back(variantName.value().position, variantName.value().value,
		                      std::move(fields), std::move(explicitValue));

		if (peek().type == Token::RightBrace)
			break;
		Result<Token> comma = expect(Token::Comma, "Expected ',' between enum variants");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after enum declaration");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<EnumDeclarationNode>(enum_keyword.value().position, enum_name.value().value,
	                                                std::move(variants), std::move(constraintType), is_union));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_union_declaration() {
	Result<Token> union_keyword = advance();
	if (!union_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(union_keyword.error());
	}

	Result<Token> union_name = expect(Token::Identifier, "Expected union name");
	if (!union_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(union_name.error());
	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after union name");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	std::vector<UnionDeclarationNode::UnionField> fields;

	// Parse fields: fieldName: Type, ...
	while (peek().type != Token::RightBrace) {
		Token::Position fieldPos = peek().position;
		Result<Token> fieldName = expect(Token::Identifier, "Expected field name");
		if (!fieldName.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(fieldName.error());
		}

		Result<Token> colon = expect(Token::Colon, "Expected ':' after field name");
		if (!colon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(colon.error());
		}

		Result<std::unique_ptr<TypeNode>> fieldType = parse_type();
		if (!fieldType.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(fieldType.error());
		}

		fields.emplace_back(fieldPos, fieldName.value().value, std::move(fieldType.value()));

		if (peek().type == Token::RightBrace)
			break;
		Result<Token> comma = expect(Token::Comma, "Expected ',' between union fields");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after union declaration");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<UnionDeclarationNode>(union_keyword.value().position, union_name.value().value,
	                                                 std::move(fields)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_constraint_declaration() {
	Token::Position start_pos = peek().position;
	Result<Token> constraint_keyword = advance();
	if (!constraint_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constraint_keyword.error());
	}

	Token::Position name_pos = peek().position;
	Result<Token> constraint_name = expect(Token::Identifier, "Expected constraint name");
	if (!constraint_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constraint_name.error());
	}

	// Parse generic parameters if present
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if (peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if (!less.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(less.error());
		}

		while (peek().type != Token::Greater) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parse_generic_parameter();
			if (!param.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(param.error());
			}

			genericParams.push_back(std::move(param.value()));

			if (peek().type == Token::Greater)
				break;
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between generic parameters");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if (!greater.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(greater.error());
		}
	}

	Token::Position assignPos = peek().position;
	Result<Token> assign = expect(Token::Assign, "Expected '=' after constraint declaration");
	if (!assign.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(assign.error());
	}

	Token::Position expr_pos = peek().position;
	Result<std::unique_ptr<ASTNode>> constraintExpressionRes = parse_expression();
	if (!constraintExpressionRes.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constraintExpressionRes.error());
	}

	std::unique_ptr<ExpressionNode> constraintExpression =
	    dynamic_unique_cast<ExpressionNode>(std::move(constraintExpressionRes.value()));

	Token::Position semicolonPos = peek().position;
	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after constraint expression");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	return Ok(std::make_unique<ConstraintDeclarationNode>(constraint_keyword.value().position,
	                                                      constraint_name.value().value, std::move(genericParams),
	                                                      std::move(constraintExpression)));
}

Result<std::unique_ptr<GenericParameter>> Parser::parse_generic_parameter() {
	Token::Position name_pos = peek().position;
	Result<Token> name = expect(Token::Identifier, "Expected generic parameter name");
	if (!name.has_value()) {
		return Err<std::unique_ptr<GenericParameter>>(name.error());
	}

	std::unique_ptr<TypeNode> constraint = nullptr;

	// Check for constraint: T: Number
	if (peek().type == Token::Colon) {
		Token::Position colonPos = peek().position;
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<GenericParameter>>(colon.error());
		}

		Token::Position constraintPos = peek().position;
		Result<std::unique_ptr<TypeNode>> constraintResult = parse_type();
		if (!constraintResult.has_value()) {
			return Err<std::unique_ptr<GenericParameter>>(constraintResult.error());
		}

		constraint = std::move(constraintResult.value());
	}

	return Ok(std::make_unique<GenericParameter>(name_pos, name.value().value, std::move(constraint)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_module_declaration() {
	Result<Token> module_keyword = advance();
	if (!module_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(module_keyword.error());
	}

	Result<Token> module_name = expect(Token::Identifier, "Expected module name");
	if (!module_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(module_name.error());
	}

	std::vector<std::string> exports;
	if (peek().type == Token::LeftBrace) {
		Result<Token> left_brace = advance();
		if (!left_brace.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(left_brace.error());
		}

		while (peek().type != Token::RightBrace) {
			Result<Token> export_name = expect(Token::Identifier, "Expected exported symbol name");
			if (!export_name.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(export_name.error());
			}
			exports.push_back(export_name.value().value);

			if (peek().type == Token::RightBrace) {
				break;
			}

			Result<Token> comma = expect(Token::Comma, "Expected ',' between exported symbols");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}

		Result<Token> right_brace = expect(Token::RightBrace, "Expected '}' after export list");
		if (!right_brace.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right_brace.error());
		}
	}

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after module declaration");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	// For now, we'll create a simple module declaration without body
	// In a full implementation, we'd parse the module body
	return Ok(std::make_unique<ModuleDeclarationNode>(module_keyword.value().position, module_name.value().value,
	                                                  std::vector<std::unique_ptr<StatementNode>>(), std::move(exports)));
}

Result<std::unique_ptr<ASTNode>> Parser::parse_import_statement() {
	Result<Token> import_keyword = advance();
	if (!import_keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(import_keyword.error());
	}

	Result<std::unique_ptr<ASTNode>> module_name = parse_member_access_expression();
	if (!module_name.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(module_name.error());
	}

	std::vector<std::string> imported_items;
	if (peek().type == Token::LeftBrace) {
		Result<Token> left_brace = advance();
		if (!left_brace.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(left_brace.error());
		}

		while (peek().type != Token::RightBrace) {
			Result<Token> item = expect(Token::Identifier, "Expected imported item name");
			if (!item.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(item.error());
			}

			imported_items.push_back(item.value().value);

			if (peek().type != Token::RightBrace) {
				Result<Token> comma = expect(Token::Comma, "Expected ',' or ';' between items");
				if (!comma.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(comma.error());
				}
			}
		}
		Result<Token> right_brace = expect(Token::RightBrace, "Expected '}' after import items");
		if (!right_brace.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right_brace.error());
		}
	}

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after import statement");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	return Ok(std::make_unique<ImportStatementNode>(import_keyword.value().position, dynamic_unique_cast<ExpressionNode>(std::move(module_name.value())),
	                                                std::move(imported_items)));
}

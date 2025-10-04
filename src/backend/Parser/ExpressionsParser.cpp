#include "backend/Parser.h"

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parsePrimary() {
	Result<Token> tokenError = advance();
	if (!tokenError.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(tokenError.error());
	}
	Token token = tokenError.value();

	if (token.type == Token::IntegralLiteral) {
		return Ok(std::make_unique<IntegralLiteralNode>(token.position, std::stoi(token.value), determineIntegerType(token.value))); // stoi needs to be changed
	} else if (token.type == Token::FloatLiteral) {
		return Ok(std::make_unique<FloatLiteralNode>(token.position, std::stod(token.value), determineFloatType(token.value))); // stod needs to be changed
	} else if (token.type == Token::StringLiteral) {
		return Ok(std::make_unique<StringLiteralNode>(token.position, token.value));
	}  else if(token.type == Token::CharLiteral) {
		return { std::make_unique<CharLiteralNode>(token.position, token.value[0]) };
	} else if(token.type == Token::BooleanLiteral) {
		return { std::make_unique<BooleanLiteralNode>(token.position, token.value == "true") };
	} else if(token.type == Token::Identifier) {
		current--;
		if(isSingleParameterLambda()) {
			return parseLambdaExpression();
		}
		current++;
		return { std::make_unique<IdentifierNode>(token.position, token.value) };
	}else if (token.type == Token::LeftParen) {
		// Step back to check for lambda expression
		current--;
		if (isLambdaExpression()) {
			return parseLambdaExpression();
		}
		// Check for single parameter lambda inside parentheses: (x -> ...)
		advance(); // consume the LeftParen
		
		// Not a lambda, parse as parenthesized expression
		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		Result<Token> tokenError1 = expect(Token::RightParen, "Expected closing ')'");
		if (!tokenError1.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(tokenError1.error());
		}

		return expr;
	}

	Error error = Error(ErrorType::UnexpectedToken,
		"Unexpected token: " + token.value + " " + Token::getTypeAsString(token.type),
		Position("", token.position.line, token.position.column))
		.withActual(token.value);
	return Err<std::unique_ptr<ASTNode>>(error);
}

Result<std::unique_ptr<ASTNode>> Parser::parseAdditiveExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMultiplicativeExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}
	while(peek().type == Token::Plus || peek().type == Token::Minus) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseMultiplicativeExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMultiplicativeExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseBitwiseNotExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::Multiply || peek().type == Token::Divide || peek().type == Token::Modulo) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseBitwiseNotExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseExpression() {
	return parseAssignmentExpression();
}

Parser::Parser(const std::vector<Token> &tokens)  : tokens(tokens) {}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalOrExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalAndExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::LogicalOr) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseLogicalAndExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalAndExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseEqualityExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::LogicalAnd) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseEqualityExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseEqualityExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseRelationalExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::Equal || peek().type == Token::NotEqual) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseRelationalExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}

		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseRelationalExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseBitwiseExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::Greater || peek().type == Token::GreaterEqual || peek().type == Token::Less || peek().type == Token::LessEqual) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseBitwiseExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}

		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseBitwiseExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseShiftExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::BitwiseOr || peek().type == Token::BitwiseAnd || peek().type == Token::BitwiseXor) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseShiftExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}

		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseShiftExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseToExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::LeftShift || peek().type == Token::RightShift) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseToExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseAssignmentExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMatchExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (!assign.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(assign.error());
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseMatchExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<AssignmentExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				assign.value(),
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parsePipeExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseParallelExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::Pipe || peek().type == Token::MapPipe) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseParallelExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMemberAccessExpression() {
	Result<std::unique_ptr<ASTNode>> left = parsePrimary();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while (peek().type == Token::Dot || peek().type == Token::DoubleColon) {
		Result<Token> accessType = advance();
		if (!accessType.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(accessType.error());
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parsePrimary();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<MemberAccessExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				accessType.value(),
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseIndexExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMemberAccessExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::LeftBracket) {
		Result<Token> leftBracket = expect(Token::LeftBracket, "Expected '['");
		if (!leftBracket.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(leftBracket.error());
		}

		Token::Position pos = peek().position;
		Token::Position leftPos = left.value()->position;
		
		// Check for slice expression (start:end) or multiple indices (idx1,idx2,idx3)
		Result<std::unique_ptr<ASTNode>> arrayExpr = parseAdvancedArrayExpression(dynamic_unique_cast<ExpressionNode>(std::move(left.value())));
		if (!arrayExpr.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arrayExpr.error());
		}

		Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']'");
		if (!rightBracket.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(rightBracket.error());
		}
		
		left = std::move(arrayExpr);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseAdvancedArrayExpression(std::unique_ptr<ExpressionNode> arrayExpr) {
	Token::Position startPos = peek().position;
	
	// Parse the first expression (could be start index or first index)
	Result<std::unique_ptr<ASTNode>> firstExpr = parseFunctionCallExpression();
	if (!firstExpr.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(firstExpr.error());
	}
	
	// Check if this is a slice expression (contains ':')
	if (peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(colon.error());
		}
		
		// Parse the end expression
		Result<std::unique_ptr<ASTNode>> endExpr = parseFunctionCallExpression();
		if (!endExpr.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(endExpr.error());
		}
		
		// Create slice expression
		return Ok(std::make_unique<SliceExpressionNode>(
			startPos,
			std::move(arrayExpr),
			dynamic_unique_cast<ExpressionNode>(std::move(firstExpr.value())),
			dynamic_unique_cast<ExpressionNode>(std::move(endExpr.value())),
			true // inclusive by default
		));
	}
	
	// Check if this is a multiple index expression (contains ',')
	std::vector<std::unique_ptr<ExpressionNode>> indices;
	indices.push_back(dynamic_unique_cast<ExpressionNode>(std::move(firstExpr.value())));
	
	while (peek().type == Token::Comma) {
		Result<Token> comma = advance();
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
		
		Result<std::unique_ptr<ASTNode>> nextExpr = parseFunctionCallExpression();
		if (!nextExpr.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(nextExpr.error());
		}
		
		indices.push_back(dynamic_unique_cast<ExpressionNode>(std::move(nextExpr.value())));
	}
	
	// If we have multiple indices, create MultipleIndexExpression
	if (indices.size() > 1) {
		return Ok(std::make_unique<MultipleIndexExpressionNode>(
			startPos,
			std::move(arrayExpr),
			std::move(indices)
		));
	}
	
	// Single index - create regular IndexExpression
	return Ok(std::make_unique<IndexExpressionNode>(
		startPos,
		std::move(arrayExpr),
		std::move(indices[0])
	));
}

Result<std::unique_ptr<ASTNode>> Parser::parseFilterExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMapExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::FilterRange) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseMapExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMapExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseReduceExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::MapRange) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseReduceExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseReduceExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalOrExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::ReduceRange || peek().type == Token::AccumulateRange) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseLogicalOrExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.value()))
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseRangeExpression() {
	if(peek().type != Token::LeftBracket) return parseFunctionCallExpression();

	Result<Token> leftBracket = advance();
	if (!leftBracket.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBracket.error());
	}

	std::vector<std::unique_ptr<ExpressionNode>> elements;

	while (peek().type != Token::RightBracket) {
		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> element;
		
		// Check if this is a nested array literal
		if (peek().type == Token::LeftBracket) {
			element = parseNestedArrayLiteral();
		} else {
			element = parseFunctionCallExpression();
		}
		
		if (!element.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(element.error());
		}

		elements.push_back(dynamic_unique_cast<ExpressionNode>(std::move(element.value())));

		if(peek().type != Token::Comma) break;

		Result<Token> comma = advance();
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']' to close the range expression");
	if (!rightBracket.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBracket.error());
	}

	return Ok(std::make_unique<RangeExpressionNode>(leftBracket.value().position, std::move(elements)));

}

Result<std::unique_ptr<ASTNode>> Parser::parseNestedArrayLiteral() {
	Result<Token> leftBracket = advance();
	if (!leftBracket.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBracket.error());
	}

	std::vector<std::unique_ptr<ExpressionNode>> elements;

	while (peek().type != Token::RightBracket) {
		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> element = parseFunctionCallExpression();
		if (!element.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(element.error());
		}

		elements.push_back(dynamic_unique_cast<ExpressionNode>(std::move(element.value())));

		if(peek().type != Token::Comma) break;

		Result<Token> comma = advance();
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']' to close the nested array literal");
	if (!rightBracket.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBracket.error());
	}

	return Ok(std::make_unique<RangeExpressionNode>(leftBracket.value().position, std::move(elements)));
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionCallExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseStructExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	// Check for generic type arguments followed by function call: func<Type1, Type2>(args)
	if(peek().type == Token::Less) {
		// Look ahead to see if this is followed by a function call
		size_t saved_pos = current;
		advance(); // consume '<'
		
		// Skip over the generic arguments to see if there's a '(' after
		int angle_bracket_depth = 1;
		bool found_function_call = false;
		while(angle_bracket_depth > 0 && current < tokens.size()) {
			if(peek().type == Token::Less) angle_bracket_depth++;
			else if(peek().type == Token::Greater) angle_bracket_depth--;
			advance();
		}
		
		if(current < tokens.size() && peek().type == Token::LeftParen) {
			found_function_call = true;
		}
		
		// Restore position
		current = saved_pos;
		
		if(found_function_call) {
			std::vector<std::unique_ptr<TypeNode>> genericTypeArgs;
			Token::Position lessPos = peek().position;
			Result<Token> less = advance();
			if (!less.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(less.error());
			}

			// Parse generic type arguments
			do {
				Token::Position typePos = peek().position;
				Result<std::unique_ptr<TypeNode>> typeArg = parseType();
				if (!typeArg.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(typeArg.error());
				}
				
				genericTypeArgs.push_back(std::move(typeArg.value()));

				if(peek().type == Token::Comma) {
					Token::Position commaPos = peek().position;
					Result<Token> comma = advance();
					if (!comma.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(comma.error());
					}
				} else {
					break;
				}
			} while(true);

			Token::Position greaterPos = peek().position;
			Result<Token> greater = expect(Token::Greater, "Expected '>' after generic type arguments");
			if (!greater.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(greater.error());
			}
			
			// Now parse the function call
			if(peek().type == Token::LeftParen) {
				Result<Token> leftParen = advance();
				if (!leftParen.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(leftParen.error());
				}

				std::vector<std::unique_ptr<ExpressionNode>> arguments;
				while(peek().type != Token::RightParen) {
					Token::Position pos = peek().position;
					Result<std::unique_ptr<ASTNode>> argument = parseExpression();
					if (!argument.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(argument.error());
					}

					arguments.push_back(dynamic_unique_cast<ExpressionNode>(std::move(argument.value())));
					if(peek().type == Token::RightParen) break;

					Result<Token> comma = expect(Token::Comma, "Expected ',' between function arguments");
					if (!comma.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(comma.error());
					}
				}
				Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function arguments");
				if (!rightParen.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(rightParen.error());
				}
				
				Token::Position leftPos = left.value()->position;
				left = Ok(std::make_unique<FunctionCallExpressionNode>(
						leftPos,
						dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
						std::move(arguments),
						std::move(genericTypeArgs)
				));
			}
		}
	}

	while(peek().type == Token::LeftParen) {
		Result<Token> leftParen = advance();
		if (!leftParen.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(leftParen.error());
		}

		std::vector<std::unique_ptr<ExpressionNode>> arguments;
		while(peek().type != Token::RightParen) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<ASTNode>> argument = parseExpression();
			if (!argument.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(argument.error());
			}

			arguments.push_back(dynamic_unique_cast<ExpressionNode>(std::move(argument.value())));
			if(peek().type == Token::RightParen) break;

			Result<Token> comma = expect(Token::Comma, "Expected ',' between function arguments");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}
		Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function arguments");
		if (!rightParen.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(rightParen.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<FunctionCallExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				std::move(arguments)
		));
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseToExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseAdditiveExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::KeywordTo) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();

		bool isInclusive = peek().type == Token::Assign;
		if(isInclusive) {
			Result<Token> assign = advance();
			if (!assign.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(assign.error());
			}
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseAdditiveExpression();
		if (!right.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(right.error());
		}
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<ToExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value())),
				dynamic_unique_cast<ExpressionNode>(std::move(right.value())),
				isInclusive
		));
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseDerefExpression() {
	if(peek().type != Token::Multiply) return parseRangeExpression();

	Result<Token> derefToken = advance();
	if (!derefToken.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(derefToken.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseRangeExpression();
	if (!operand.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(operand.error());
	}

	return Ok(std::make_unique<UnaryExpressionNode>(
				derefToken.value().position,
				derefToken.value(),
				dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))));
}

Result<std::unique_ptr<ASTNode>> Parser::parseOwnershipExpression() {
	if(peek().type != Token::Ownership) return parseReferenceExpression();

	Result<Token> ownershipToken = advance();
	if (!ownershipToken.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(ownershipToken.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseReferenceExpression();
	if (!operand.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(operand.error());
	}

	return Ok(std::make_unique<UnaryExpressionNode>(
			ownershipToken.value().position,
			ownershipToken.value(),
			dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))));
}

Result<std::unique_ptr<ASTNode>> Parser::parseReferenceExpression() {
	// Check for && (move reference) first, then & (immutable reference)
	if(peek().type == Token::LogicalAnd) {
		Result<Token> moveRefToken = advance();
		if (!moveRefToken.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(moveRefToken.error());
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> operand = parseDerefExpression();
		if (!operand.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(operand.error());
		}

		return Ok(std::make_unique<UnaryExpressionNode>(
				moveRefToken.value().position,
				moveRefToken.value(),
				dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))));
	}
	else if(peek().type == Token::MapRange) {
	Result<Token> refToken = advance();
		if (!refToken.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(refToken.error());
		}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseDerefExpression();
		if (!operand.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(operand.error());
		}

	return Ok(std::make_unique<UnaryExpressionNode>(
				refToken.value().position,
				refToken.value(),
				dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))));
	}
	
	return parseDerefExpression();
}

Result<std::unique_ptr<ASTNode>> Parser::parseIncrementExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseOwnershipExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::Increment || peek().type == Token::Decrement) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<UnaryExpressionNode>(
				leftPos,
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value()))
		));
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnaryMinusExpression() {
	if(peek().type != Token::Minus) return parseIncrementExpression();

	Result<Token> minusToken = advance();
	if (!minusToken.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(minusToken.error());
	}

	if(peek().type == Token::Plus) {
		// Decrement
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseIncrementExpression();
	if (!operand.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(operand.error());
	}

	return Ok(std::make_unique<UnaryExpressionNode>(
			minusToken.value().position,
			minusToken.value(),
			dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))));

}

Result<std::unique_ptr<ASTNode>> Parser::parseAwaitExpression() {
	if(peek().type != Token::KeywordAwait) return parseIteratorExpression();

	Result<Token> awaitToken = advance();
	if (!awaitToken.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(awaitToken.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseIteratorExpression();
	if (!operand.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(operand.error());
	}

	return Ok(std::make_unique<UnaryExpressionNode>(
			awaitToken.value().position,
			awaitToken.value(),
			dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))
	));
}

Result<std::unique_ptr<ASTNode>> Parser::parseIteratorExpression() {
	if(peek().type != Token::Dollar) return parseUnaryPlusExpression();

	Result<Token> dollarToken = advance();
	if (!dollarToken.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(dollarToken.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseUnaryPlusExpression();
	if (!operand.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(operand.error());
	}

	return Ok(std::make_unique<UnaryExpressionNode>(
			dollarToken.value().position,
			dollarToken.value(),
			dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))
	));
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnaryPlusExpression() {
	if(peek().type != Token::Plus) return parseUnaryMinusExpression();

	Result<Token> plusToken = advance();
	if (!plusToken.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(plusToken.error());
	}

	if(peek().type == Token::Plus) {
		// Increment
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseUnaryMinusExpression();
	if (!operand.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(operand.error());
	}

	return Ok(std::make_unique<UnaryExpressionNode>(
			plusToken.value().position,
			plusToken.value(),
			dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))));
}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalNotExpression() {
	// Check for prefix logical not operator
	if(peek().type == Token::LogicalNot) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();
		
		// Parse the operand
		Result<std::unique_ptr<ASTNode>> operand = parseAwaitExpression();
		if (!operand.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(operand.error());
		}
		
		return Ok(std::make_unique<UnaryExpressionNode>(
				op.position,
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(operand.value()))
		));
	}
	
	// No logical not operator, continue with next level
	return parseAwaitExpression();
}

Result<std::unique_ptr<ASTNode>> Parser::parseBitwiseNotExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalNotExpression();
	if (!left.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(left.error());
	}

	while(peek().type == Token::BitwiseNot) {
		Result<Token> opError = advance();
		if (!opError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(opError.error());
		}
		Token op = opError.value();
		Token::Position leftPos = left.value()->position;
		left = Ok(std::make_unique<UnaryExpressionNode>(
				leftPos,
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(left.value()))
		));
	}
	return left;
}



Result<std::unique_ptr<ASTNode>> Parser::parseParallelExpression() {
	if(peek().type != Token::KeywordPar) return parseFilterExpression();

	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Token::Position pos = peek().position;
	
	// Check if this is a par with configuration: par(struct { ... }) expression
	if(peek().type == Token::LeftParen) {
		// For now, skip the configuration parsing and just parse the expression after it
		// TODO: Implement configuration parsing
		Result<std::unique_ptr<ASTNode>> expr = parseFilterExpression();
		if (!expr.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(expr.error());
		}
	return Ok(std::make_unique<ParallelExpressionNode>(
				token.value().position,
				std::move(expr.value())));
	}
		// Parse as expression, not statement - use a lower level to avoid recursion
	Result<std::unique_ptr<ASTNode>> expr = parseFilterExpression();
	if (!expr.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(expr.error());
	}
	return Ok(std::make_unique<ParallelExpressionNode>(
			token.value().position,
			std::move(expr.value())));

}

Result<std::unique_ptr<ASTNode>> Parser::parseStructExpression() {
	if(peek().type != Token::KeywordStruct) return parseIndexExpression();

	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after struct");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	std::vector<StructField> fields;
	while(peek().type != Token::RightBrace) {
		Token::Position namePos = peek().position;
		Result<Token> nameError = expect(Token::Identifier, "Expected fields base");
		if (!nameError.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(nameError.error());
		}
		Token name = nameError.value();

		Result<std::unique_ptr<TypeNode>> type;
		Result<std::unique_ptr<ASTNode>> expression;

		if(peek().type == Token::Colon) {
			Token::Position colonPos = peek().position;
			Result<Token> token1 = advance();
			if (!token1.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(token1.error());
			}

			Token::Position pos = peek().position;
			type = parseType();
			if (!type.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(type.error());
			}
		}

		if(peek().type == Token::Assign) {
			Token::Position assignPos = peek().position;
			Result<Token> token1 = advance();
			if (!token1.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(token1.error());
			}

			Token::Position pos = peek().position;
			expression = parseExpression();
			if (!expression.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(expression.error());
			}
		}

		if (!type.has_value() && !expression.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(create_parse_error(ErrorType::UnexpectedToken, "Cannot have field without value or type", peek().position));
		}

		if(peek().type != Token::RightBrace) {
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between fields");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}

		std::unique_ptr<TypeNode> typeValue = type.has_value() ? std::move(type.value()) : nullptr;
		std::unique_ptr<ExpressionNode> expressionValue = expression.has_value() ? dynamic_unique_cast<ExpressionNode>(std::move(expression.value())) : nullptr;

		fields.emplace_back(
				name.position,
				name.value,
				std::move(typeValue),
				std::move(expressionValue));
	}

	advance();

	return { std::make_unique<StructExpressionNode>(token.value().position, std::move(fields)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseMatchExpression() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> value = parsePipeExpression();
	if (!value.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(value.error());
	}
	if(peek().type != Token::MatchArrow) return value;
	
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after match expression");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	std::vector<std::unique_ptr<MatchBranch>> branches;
	while(peek().type != Token::RightBrace) {

		Token::Position patternPos = peek().position;
		Result<std::unique_ptr<PatternNode>> parsedPattern = parsePattern();
		if (!parsedPattern.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(parsedPattern.error());
		}
		
		// Parse optional guard condition (&&)
		std::unique_ptr<ExpressionNode> condition = nullptr;
		if(peek().type == Token::LogicalAnd) {
			advance(); // consume &&
			Result<std::unique_ptr<ASTNode>> guardExpr = parseExpression();
			if (!guardExpr.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(guardExpr.error());
			}
			condition = dynamic_unique_cast<ExpressionNode>(std::move(guardExpr.value()));
		}

		Result<Token> arrow = expect(Token::Arrow, "Expected '->' after pattern");
		if (!arrow.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arrow.error());
		}

		Result<std::unique_ptr<ASTNode>> body = parseExpression();
		Token::Position bodyPos = peek().position;
		if (!body.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(body.error());
		}

		branches.push_back(std::make_unique<MatchBranch>(
				patternPos,
			std::move(parsedPattern.value()),
			std::move(condition),
			dynamic_unique_cast<ExpressionNode>(std::move(body.value()))));

		if(peek().type == Token::RightBrace) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' or '}'");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after match statement");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}


	return Ok(std::make_unique<MatchExpressionNode>(
			pos,
			dynamic_unique_cast<ExpressionNode>(std::move(value.value())), std::move(branches)));
}

bool Parser::isLambdaExpression() {
	if (peek().type != Token::LeftParen) return false;

	size_t startIndex = current;

	Result<Token> leftParen = advance();
	if (!leftParen.has_value()) {
		current = startIndex;
		return false;
	}

	while (peek().type != Token::RightParen) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			current = startIndex;
			return false;
		}
	}

	Result<Token> rightParen = advance();
	if (!rightParen.has_value()) {
		current = startIndex;
		return false;
	}

	if(peek().type != Token::Arrow) {
		current = startIndex;
		return false;
	}

	current = startIndex;
	return true;
}

bool Parser::isSingleParameterLambda() {
	if (peek().type != Token::Identifier) return false;
	
	size_t startIndex = current;
	
	// Skip the identifier
	Result<Token> identifier = advance();
	if (!identifier.has_value()) {
		current = startIndex;
		return false;
	}
	
	// Check if next token is arrow
	bool isLambda = peek().type == Token::Arrow;
	
	current = startIndex;
	return isLambda;
}


Result<std::unique_ptr<ASTNode>> Parser::parseLambdaExpression() {
	if(!isLambdaExpression() && !isSingleParameterLambda()) return parseStructExpression();
	
	std::vector<std::unique_ptr<FunctionArgument>> args;
	Token::Position startPos = peek().position;
	
	if(peek().type == Token::LeftParen) {
		// Handle (x) -> ... or (x, y) -> ... syntax
		Result<Token> leftParen = advance();
		if (!leftParen.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(leftParen.error());
		}
		startPos = leftParen.value().position;

	while(peek().type != Token::RightParen) {
		Token::Position argPos = peek().position;
		Result<std::unique_ptr<FunctionArgument>> arg = parseFunctionArgument();
			if (!arg.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(arg.error());
			}

			args.push_back(std::move(arg.value()));

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
			if (!comma.has_value()) return Err<std::unique_ptr<ASTNode>>(comma.error());
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if (!rightParen.has_value()) return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	} else {
		// Handle x -> ... syntax (single parameter without parentheses)
		Result<Token> identifier = expect(Token::Identifier, "Expected parameter name");
		if (!identifier.has_value()) return Err<std::unique_ptr<ASTNode>>(identifier.error());
		startPos = identifier.value().position;
		
		auto arg = std::make_unique<FunctionArgument>(identifier.value().position, nullptr, nullptr, identifier.value().value);
		args.push_back(std::move(arg));
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->'");
	if (!arrow.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(arrow.error());
	}

	Result<std::unique_ptr<ASTNode>> body = parseExpression();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	auto bodyPosition = body.value()->position;
		return Ok(std::make_unique<LambdaExpressionNode>(
			startPos,
				std::move(args),
				std::make_unique<ReturnStatementNode>(bodyPosition,
										  dynamic_unique_cast<ExpressionNode>(std::move(body.value())), false)));
}

// Pattern parsing implementations
Result<std::unique_ptr<PatternNode>> Parser::parsePattern() {
	Token::Position pos = peek().position;

	
	switch(peek().type) {
		case Token::Identifier:
			// Could be identifier pattern, constructor pattern, or type pattern
			if (peek(1).type == Token::LeftParen) {
				// Constructor pattern: Point(x, y)
				return parseConstructorPattern();
			} else if (peek(1).type == Token::DoubleColon) {
				// Enum constructor: Shape::Circle(r)
				return parseConstructorPattern();
			} else if (peek().value == "_") {
				// Wildcard pattern
				return parseWildcardPattern();
			} else {
				// Identifier pattern or type pattern
				return parseIdentifierPattern();
			}
		case Token::IntegralLiteral:
		case Token::FloatLiteral:
		case Token::StringLiteral:
		case Token::BooleanLiteral:
		case Token::CharLiteral:
			return parseLiteralPattern();
		case Token::LeftBracket:
			return parseArrayPattern();
		case Token::LeftBrace:
			return parseStructPattern();
		case Token::PrimitiveType:
			return parseTypePattern();
		default:
			// Try to parse as range pattern or fallback to literal
			return parseLiteralPattern();
	}
}

Result<std::unique_ptr<PatternNode>> Parser::parseWildcardPattern() {
	Result<Token> underscore = expect(Token::Identifier, "Expected '_'");
	if (!underscore.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(underscore.error());
	}
	
	if(underscore.value().value != "_") {
		return Err<std::unique_ptr<PatternNode>>(create_parse_error(ErrorType::UnexpectedToken, "Expected '_' for wildcard pattern", underscore.value().position));
	}
	
	return Ok(std::make_unique<WildcardPatternNode>(underscore.value().position));
}

Result<std::unique_ptr<PatternNode>> Parser::parseLiteralPattern() {
	Result<std::unique_ptr<ASTNode>> literal = parsePrimary();
	if (!literal.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(literal.error());
	}
	
	// Check if this is a range pattern (literal to literal)
	if(peek().type == Token::KeywordTo) {
		current--; // Go back to re-parse as range
		return parseRangePattern();
	}
	
	Token::Position literalPos = literal.value()->position;
	return Ok(std::make_unique<LiteralPatternNode>(
		literalPos,
		dynamic_unique_cast<ExpressionNode>(std::move(literal.value()))
	));
}

Result<std::unique_ptr<PatternNode>> Parser::parseIdentifierPattern() {
	Result<Token> identifier = expect(Token::Identifier, "Expected identifier");
	if (!identifier.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(identifier.error());
	}
	
	return Ok(std::make_unique<IdentifierPatternNode>(
		identifier.value().position,
		identifier.value().value
	));
}

Result<std::unique_ptr<PatternNode>> Parser::parseArrayPattern() {
	Token::Position pos = peek().position;
	Result<Token> leftBracket = expect(Token::LeftBracket, "Expected '['");
	if (!leftBracket.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(leftBracket.error());
	}
	
	std::vector<std::unique_ptr<PatternNode>> elements;
	std::unique_ptr<PatternNode> rest = nullptr;
	
	while(peek().type != Token::RightBracket) {
		if(peek().type == Token::Ellipsis) {
			// Rest pattern: ...rest
			advance(); // consume ...
			if(peek().type == Token::Identifier) {
			Result<std::unique_ptr<PatternNode>> restPattern = parsePattern();
			if (!restPattern.has_value()) {
				return Err<std::unique_ptr<PatternNode>>(restPattern.error());
			}
			rest = std::move(restPattern.value());
			}
			break;
		}
		
		Result<std::unique_ptr<PatternNode>> element = parsePattern();
		if (!element.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(element.error());
		}
		elements.push_back(std::move(element.value()));
		
		if(peek().type == Token::RightBracket) break;
		
		Result<Token> comma = expect(Token::Comma, "Expected ',' or ']'");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(comma.error());
		}
	}
	
	Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']'");
	if (!rightBracket.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(rightBracket.error());
		}
	
	return Ok(std::make_unique<ArrayPatternNode>(pos, std::move(elements), std::move(rest)));
}

Result<std::unique_ptr<PatternNode>> Parser::parseStructPattern() {
	Token::Position pos = peek().position;
	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{'");
	if (!leftBrace.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(leftBrace.error());
		}
	
	std::vector<std::pair<std::string, std::unique_ptr<PatternNode>>> fields;
	
	while(peek().type != Token::RightBrace) {
		Result<Token> fieldName = expect(Token::Identifier, "Expected field name");
		if (!fieldName.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(fieldName.error());
		}
		
		std::unique_ptr<PatternNode> pattern;
		if(peek().type == Token::Colon) {
			advance(); // consume :
			Result<std::unique_ptr<PatternNode>> fieldPattern = parsePattern();
			if (!fieldPattern.has_value()) {
				return Err<std::unique_ptr<PatternNode>>(fieldPattern.error());
			}
			pattern = std::move(fieldPattern.value());
		} else {
			// Shorthand: {x} is equivalent to {x: x}
			pattern = std::make_unique<IdentifierPatternNode>(fieldName.value().position, fieldName.value().value);
		}
		
		fields.emplace_back(fieldName.value().value, std::move(pattern));
		
		if(peek().type == Token::RightBrace) break;
		
		Result<Token> comma = expect(Token::Comma, "Expected ',' or '}'");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(comma.error());
		}
	}
	
	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}'");
	if (!rightBrace.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(rightBrace.error());
		}
	
	return Ok(std::make_unique<StructPatternNode>(pos, std::move(fields)));
}

Result<std::unique_ptr<PatternNode>> Parser::parseConstructorPattern() {
	Token::Position pos = peek().position;
	Result<Token> name = expect(Token::Identifier, "Expected constructor name");
	if (!name.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(name.error());
	}
	
	std::string constructorName = name.value().value;
	
	// Handle enum constructors: Shape::Circle
	if(peek().type == Token::DoubleColon) {
		advance(); // consume ::
		Result<Token> enumName = expect(Token::Identifier, "Expected enum variant name");
		if (!enumName.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(enumName.error());
		}
		constructorName += "::" + enumName.value().value;
	}
	
	std::vector<std::unique_ptr<PatternNode>> arguments;
	
	if(peek().type == Token::LeftParen) {
		advance(); // consume (
		
		while(peek().type != Token::RightParen) {
			Result<std::unique_ptr<PatternNode>> arg = parsePattern();
			if (!arg.has_value()) {
				return Err<std::unique_ptr<PatternNode>>(arg.error());
			}
			arguments.push_back(std::move(arg.value()));
			
			if(peek().type == Token::RightParen) break;
			
			Result<Token> comma = expect(Token::Comma, "Expected ',' or ')'");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<PatternNode>>(comma.error());
			}
		}
		
		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if (!rightParen.has_value()) {
			return Err<std::unique_ptr<PatternNode>>(rightParen.error());
		}
	}
	
	return Ok(std::make_unique<ConstructorPatternNode>(pos, constructorName, std::move(arguments)));
}

Result<std::unique_ptr<PatternNode>> Parser::parseTypePattern() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> type = parseType();
	if (!type.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(type.error());
	}
	
	return Ok(std::make_unique<TypePatternNode>(pos, std::move(type.value())));
}

Result<std::unique_ptr<PatternNode>> Parser::parseRangePattern() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> start = parsePrimary();
	if (!start.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(start.error());
	}
	
	bool isInclusive = false;
	if(peek().type == Token::KeywordTo) {
		advance(); // consume 'to'
		if(peek().type == Token::Assign) {
			advance(); // consume '='
			isInclusive = true;
		}
	} else {
		return Err<std::unique_ptr<PatternNode>>(create_parse_error(ErrorType::UnexpectedToken, "Expected 'to' in range pattern", peek().position));
	}
	
	Result<std::unique_ptr<ASTNode>> end = parsePrimary();
	if (!end.has_value()) {
		return Err<std::unique_ptr<PatternNode>>(end.error());
	}
	
	return Ok(std::make_unique<RangePatternNode>(
		pos,
		dynamic_unique_cast<ExpressionNode>(std::move(start.value())),
		dynamic_unique_cast<ExpressionNode>(std::move(end.value())),
		isInclusive
	));
}

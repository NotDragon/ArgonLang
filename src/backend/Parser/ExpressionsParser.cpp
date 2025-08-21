#include "backend/Parser.h"

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parsePrimary() {
	Result<Token> tokenError = advance();
	if(tokenError.hasError()) return { tokenError, Trace(ASTNodeType::StringLiteral, tokenError.getValue().position) };
	Token token = tokenError.getValue();

	if (token.type == Token::IntegralLiteral) {
		return { std::make_unique<IntegralLiteralNode>(token.position, std::stoi(token.value), determineIntegerType(token.value)) }; // stoi needs to be changed
	} else if (token.type == Token::FloatLiteral) {
		return { std::make_unique<FloatLiteralNode>(token.position, std::stod(token.value), determineFloatType(token.value)) }; // stod needs to be changed
	} else if (token.type == Token::StringLiteral) {
		return { std::make_unique<StringLiteralNode>(token.position, token.value) };
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
		if(tokenError1.hasError()) return { tokenError1, Trace(ASTNodeType::StringLiteral, tokenError1.getValue().position)};

		return expr;
	}

	return { "Unexpected token: " + token.value + " " + Token::getTypeAsString(token.type), "", Trace(ASTNodeType::StringLiteral, token.position) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseAdditiveExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMultiplicativeExpression();
	if(left.hasError()) return left;
	while(peek().type == Token::Plus || peek().type == Token::Minus) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseMultiplicativeExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.moveValue())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.moveValue()))
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMultiplicativeExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseBitwiseNotExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Multiply || peek().type == Token::Divide || peek().type == Token::Modulo) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseBitwiseNotExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(std::move(left.moveValue())),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right.moveValue()))
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseExpression() {
	return parseAssignmentExpression();
}

Parser::Parser(const std::vector<Token> &tokens)  : tokens(tokens) {}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalOrExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalAndExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::LogicalOr) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseLogicalAndExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalAndExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseEqualityExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::LogicalAnd) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseEqualityExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseEqualityExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseRelationalExpression();

	while(peek().type == Token::Equal || peek().type == Token::NotEqual) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseRelationalExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};

		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseRelationalExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseBitwiseExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Greater || peek().type == Token::GreaterEqual || peek().type == Token::Less || peek().type == Token::LessEqual) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseBitwiseExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};

		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseBitwiseExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseShiftExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::BitwiseOr || peek().type == Token::BitwiseAnd || peek().type == Token::BitwiseXor) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseShiftExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};

		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseShiftExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseToExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::LeftShift || peek().type == Token::RightShift) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseToExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseAssignmentExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMatchExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (assign.hasError()) return { assign, Trace(ASTNodeType::AssignmentExpression, assign.getValue().position) };

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseMatchExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::AssignmentExpression, pos) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<AssignmentExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				assign.getValue(),
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parsePipeExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseParallelExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Pipe || peek().type == Token::MapPipe) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseParallelExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMemberAccessExpression() {
	Result<std::unique_ptr<ASTNode>> left = parsePrimary();
	if(left.hasError()) return left;

	while (peek().type == Token::Dot || peek().type == Token::DoubleColon) {
		Result<Token> accessType = advance();
		if (accessType.hasError())
			return {accessType,
					Trace(ASTNodeType::MemberAccessExpression, accessType.getValue().position)};

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parsePrimary();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::MemberAccessExpression, pos) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<MemberAccessExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				accessType.getValue(),
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseIndexExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMemberAccessExpression();
	if(left.hasError())	return left;

	while(peek().type == Token::LeftBracket) {
		Result<Token> leftBracket = expect(Token::LeftBracket, "Expected '['");
		if (leftBracket.hasError()) return {leftBracket, Trace(ASTNodeType::IndexExpression, leftBracket.getValue().position) };

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> index = parseMemberAccessExpression();
		if (index.hasError()) return { std::move(index), Trace(ASTNodeType::IndexExpression, pos) };

		Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']'");
		if (rightBracket.hasError()) return {rightBracket, Trace(ASTNodeType::IndexExpression, rightBracket.getValue().position) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<IndexExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				dynamic_unique_cast<ExpressionNode>(index.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseFilterExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMapExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::FilterRange) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseMapExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMapExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseReduceExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::MapRange) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseReduceExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseReduceExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalOrExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::ReduceRange || peek().type == Token::AccumulateRange) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::BinaryExpression, opError.getValue().position) };
		Token op = opError.getValue();

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseLogicalOrExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::BinaryExpression, pos)};
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<BinaryExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseRangeExpression() {
	if(peek().type != Token::LeftBracket) return parseFunctionCallExpression();

	Result<Token> leftBracket = advance();
	if (leftBracket.hasError()) return {leftBracket, Trace(ASTNodeType::RangeExpression, leftBracket.getValue().position) };

	std::vector<std::unique_ptr<ExpressionNode>> elements;

	while (peek().type != Token::RightBracket) {
		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> element = parseFunctionCallExpression();
		if (element.hasError()) return { std::move(element), Trace(ASTNodeType::RangeExpression, pos) };

		elements.push_back(dynamic_unique_cast<ExpressionNode>(element.moveValue()));

		if(peek().type != Token::Comma) break;

		Result<Token> comma = advance();
		if (comma.hasError()) return {comma, Trace(ASTNodeType::RangeExpression, comma.getValue().position) };
	}

	Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']' to close the range expression");
	if (rightBracket.hasError()) return {rightBracket, Trace(ASTNodeType::RangeExpression, rightBracket.getValue().position) };

	return { std::make_unique<RangeExpressionNode>(leftBracket.getValue().position, std::move(elements)) };

}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionCallExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseStructExpression();
	if(left.hasError()) return left;

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
			if(less.hasError()) return { less, Trace(ASTNodeType::FunctionCallExpression, lessPos) };

			// Parse generic type arguments
			do {
				Token::Position typePos = peek().position;
				Result<std::unique_ptr<TypeNode>> typeArg = parseType();
				if(typeArg.hasError()) return { std::move(typeArg), Trace(ASTNodeType::FunctionCallExpression, typePos) };
				
				genericTypeArgs.push_back(typeArg.moveValue());

				if(peek().type == Token::Comma) {
					Token::Position commaPos = peek().position;
					Result<Token> comma = advance();
					if(comma.hasError()) return { comma, Trace(ASTNodeType::FunctionCallExpression, commaPos) };
				} else {
					break;
				}
			} while(true);

			Token::Position greaterPos = peek().position;
			Result<Token> greater = expect(Token::Greater, "Expected '>' after generic type arguments");
			if(greater.hasError()) return { greater, Trace(ASTNodeType::FunctionCallExpression, greaterPos) };
			
			// Now parse the function call
			if(peek().type == Token::LeftParen) {
				Result<Token> leftParen = advance();
				if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::FunctionCallExpression, leftParen.getValue().position) };

				std::vector<std::unique_ptr<ExpressionNode>> arguments;
				while(peek().type != Token::RightParen) {
					Token::Position pos = peek().position;
					Result<std::unique_ptr<ASTNode>> argument = parseExpression();
					if(argument.hasError()) return { std::move(argument), Trace(ASTNodeType::FunctionCallExpression, pos)};

					arguments.push_back(dynamic_unique_cast<ExpressionNode>(argument.moveValue()));
					if(peek().type == Token::RightParen) break;

					Result<Token> comma = expect(Token::Comma, "Expected ',' between function arguments");
					if(comma.hasError()) return { comma, Trace(ASTNodeType::FunctionCallExpression, comma.getValue().position) };
				}
				Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function arguments");
				if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::FunctionCallExpression, rightParen.getValue().position) };
				
				Token::Position leftPos = left.getValue()->position;
				left = std::make_unique<FunctionCallExpressionNode>(
						leftPos,
						dynamic_unique_cast<ExpressionNode>(left.moveValue()),
						std::move(arguments),
						std::move(genericTypeArgs)
				);
			}
		}
	}

	while(peek().type == Token::LeftParen) {
		Result<Token> leftParen = advance();
		if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::FunctionCallExpression, leftParen.getValue().position) };

		std::vector<std::unique_ptr<ExpressionNode>> arguments;
		while(peek().type != Token::RightParen) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<ASTNode>> argument = parseExpression();
			if(argument.hasError()) return { std::move(argument), Trace(ASTNodeType::FunctionCallExpression, pos)};

			arguments.push_back(dynamic_unique_cast<ExpressionNode>(argument.moveValue()));
			if(peek().type == Token::RightParen) break;

			Result<Token> comma = expect(Token::Comma, "Expected ',' between function arguments");
			if(comma.hasError()) return { comma, Trace(ASTNodeType::FunctionCallExpression, comma.getValue().position) };
		}
		Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function arguments");
		if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::FunctionCallExpression, rightParen.getValue().position) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<FunctionCallExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				std::move(arguments)
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseToExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseAdditiveExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::KeywordTo) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::ToExpression, opError.getValue().position) };
		Token op = opError.getValue();

		bool isInclusive = peek().type == Token::Assign;
		if(isInclusive) {
			Result<Token> assign = advance();
			if(assign.hasError()) return { assign, Trace(ASTNodeType::ToExpression, assign.getValue().position) };
		}

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseAdditiveExpression();
		if(right.hasError()) return { std::move(right), Trace(ASTNodeType::ToExpression, pos) };
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<ToExpressionNode>(
				leftPos,
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				dynamic_unique_cast<ExpressionNode>(right.moveValue()),
				isInclusive
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseDerefExpression() {
	if(peek().type != Token::Multiply) return parseRangeExpression();

	Result<Token> derefToken = advance();
	if (derefToken.hasError()) return { derefToken, Trace(ASTNodeType::UnaryExpression, derefToken.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseRangeExpression();
	if (operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
				derefToken.getValue().position,
				derefToken.getValue(),
				dynamic_unique_cast<ExpressionNode>(operand.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseOwnershipExpression() {
	if(peek().type != Token::Ownership) return parseReferenceExpression();

	Result<Token> ownershipToken = advance();
	if(ownershipToken.hasError()) return { ownershipToken, Trace(ASTNodeType::UnaryExpression, ownershipToken.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseReferenceExpression();
	if(operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
			ownershipToken.getValue().position,
			ownershipToken.getValue(),
			dynamic_unique_cast<ExpressionNode>(operand.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseReferenceExpression() {
	// Check for && (move reference) first, then & (immutable reference)
	if(peek().type == Token::LogicalAnd) {
		Result<Token> moveRefToken = advance();
		if(moveRefToken.hasError()) return { moveRefToken, Trace(ASTNodeType::UnaryExpression, moveRefToken.getValue().position) };

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> operand = parseDerefExpression();
		if(operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

		return { std::make_unique<UnaryExpressionNode>(
				moveRefToken.getValue().position,
				moveRefToken.getValue(),
				dynamic_unique_cast<ExpressionNode>(operand.moveValue())) };
	}
	else if(peek().type == Token::MapRange) {
		Result<Token> refToken = advance();
		if(refToken.hasError()) return { refToken, Trace(ASTNodeType::UnaryExpression, refToken.getValue().position) };

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> operand = parseDerefExpression();
		if(operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

		return { std::make_unique<UnaryExpressionNode>(
				refToken.getValue().position,
				refToken.getValue(),
				dynamic_unique_cast<ExpressionNode>(operand.moveValue())) };
	}
	
	return parseDerefExpression();
}

Result<std::unique_ptr<ASTNode>> Parser::parseIncrementExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseOwnershipExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Increment || peek().type == Token::Decrement) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::UnaryExpression, opError.getValue().position) };
		Token op = opError.getValue();
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<UnaryExpressionNode>(
				leftPos,
				op,
				dynamic_unique_cast<ExpressionNode>(left.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnaryMinusExpression() {
	if(peek().type != Token::Minus) return parseIncrementExpression();

	Result<Token> minusToken = advance();
	if (minusToken.hasError()) return { minusToken, Trace(ASTNodeType::UnaryExpression, minusToken.getValue().position) };

	if(peek().type == Token::Plus) {
		// Decrement
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseIncrementExpression();
	if (operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
			minusToken.getValue().position,
			minusToken.getValue(),
			dynamic_unique_cast<ExpressionNode>(operand.moveValue()))
	};

}

Result<std::unique_ptr<ASTNode>> Parser::parseAwaitExpression() {
	if(peek().type != Token::KeywordAwait) return parseIteratorExpression();

	Result<Token> awaitToken = advance();
	if(awaitToken.hasError()) return { awaitToken, Trace(ASTNodeType::UnaryExpression, awaitToken.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseIteratorExpression();
	if(operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
			awaitToken.getValue().position,
			awaitToken.getValue(),
			dynamic_unique_cast<ExpressionNode>(operand.moveValue())
	) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseIteratorExpression() {
	if(peek().type != Token::Dollar) return parseUnaryPlusExpression();

	Result<Token> dollarToken = advance();
	if(dollarToken.hasError()) return { dollarToken, Trace(ASTNodeType::UnaryExpression, dollarToken.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseUnaryPlusExpression();
	if(operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
			dollarToken.getValue().position,
			dollarToken.getValue(),
			dynamic_unique_cast<ExpressionNode>(operand.moveValue())
	) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnaryPlusExpression() {
	if(peek().type != Token::Plus) return parseUnaryMinusExpression();

	Result<Token> plusToken = advance();
	if (plusToken.hasError()) return { plusToken, Trace(ASTNodeType::UnaryExpression, plusToken.getValue().position) };

	if(peek().type == Token::Plus) {
		// Increment
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseUnaryMinusExpression();
	if (operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
			plusToken.getValue().position,
			plusToken.getValue(),
			dynamic_unique_cast<ExpressionNode>(operand.moveValue()))
	};
}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalNotExpression() {
	// Check for prefix logical not operator
	if(peek().type == Token::LogicalNot) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::UnaryExpression, opError.getValue().position) };
		Token op = opError.getValue();
		
		// Parse the operand
		Result<std::unique_ptr<ASTNode>> operand = parseAwaitExpression();
		if(operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, op.position) };
		
		return { std::make_unique<UnaryExpressionNode>(
				op.position,
				op,
				dynamic_unique_cast<ExpressionNode>(operand.moveValue())
		) };
	}
	
	// No logical not operator, continue with next level
	return parseAwaitExpression();
}

Result<std::unique_ptr<ASTNode>> Parser::parseBitwiseNotExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalNotExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::BitwiseNot) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError, Trace(ASTNodeType::UnaryExpression, opError.getValue().position) };
		Token op = opError.getValue();
		Token::Position leftPos = left.getValue()->position;
		left = std::make_unique<UnaryExpressionNode>(
				leftPos,
				op,
				dynamic_unique_cast<ExpressionNode>(left.moveValue())
		);
	}
	return left;
}



Result<std::unique_ptr<ASTNode>> Parser::parseParallelExpression() {
	if(peek().type != Token::KeywordPar) return parseFilterExpression();

	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::UnaryExpression, token.getValue().position) };

	Token::Position pos = peek().position;
	
	// Check if this is a par with configuration: par(struct { ... }) expression
	if(peek().type == Token::LeftParen) {
		// For now, skip the configuration parsing and just parse the expression after it
		// TODO: Implement configuration parsing
		Result<std::unique_ptr<ASTNode>> expr = parseFilterExpression();
		if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::ParallelExpression, pos) };
		return { std::make_unique<ParallelExpressionNode>(
				token.getValue().position,
				expr.moveValue())
		};
	} else {
		// Parse as expression, not statement - use a lower level to avoid recursion
		Result<std::unique_ptr<ASTNode>> expr = parseFilterExpression();
		if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::ParallelExpression, pos) };
		return { std::make_unique<ParallelExpressionNode>(
				token.getValue().position,
				expr.moveValue())
		};
	}
}

Result<std::unique_ptr<ASTNode>> Parser::parseStructExpression() {
	if(peek().type != Token::KeywordStruct) return parseIndexExpression();

	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::StructExpression, token.getValue().position) };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after struct");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::StructExpression, leftBrace.getValue().position) };

	std::vector<StructField> fields;
	while(peek().type != Token::RightBrace) {
		Token::Position namePos = peek().position;
		Result<Token> nameError = expect(Token::Identifier, "Expected fields base");
		if(nameError.hasError()) return { nameError, Trace(ASTNodeType::StructExpression, namePos) };
		Token name = nameError.getValue();

		Result<std::unique_ptr<TypeNode>> type;
		Result<std::unique_ptr<ASTNode>> expression;

		if(peek().type == Token::Colon) {
			Token::Position colonPos = peek().position;
			Result<Token> token1 = advance();
			if(token1.hasError()) return { token1, Trace(ASTNodeType::StructExpression, colonPos) };

			Token::Position pos = peek().position;
			type = parseType();
			if(type.hasError()) return { std::move(type), Trace(ASTNodeType::StructExpression, pos) };
		}

		if(peek().type == Token::Assign) {
			Token::Position assignPos = peek().position;
			Result<Token> token1 = advance();
			if(token1.hasError()) return { token1, Trace(ASTNodeType::StructExpression, assignPos) };

			Token::Position pos = peek().position;
			expression = parseExpression();
			if(expression.hasError()) return { std::move(expression), Trace(ASTNodeType::StructExpression, pos) };
		}

		if(type.isNull() && expression.isNull()) {
			return { "Cannot have field without value or type", "", Trace(ASTNodeType::StructExpression, peek().position) };
		}

		if(peek().type != Token::RightBrace) {
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between fields");
			if(comma.hasError()) return { comma, Trace(ASTNodeType::StructExpression, commaPos) };
		}

		std::unique_ptr<TypeNode> typeValue = !type.isNull() ? type.moveValue() : nullptr;
		std::unique_ptr<ExpressionNode> expressionValue = !expression.isNull() ? dynamic_unique_cast<ExpressionNode>(expression.moveValue()) : nullptr;

		fields.emplace_back(
				name.position,
				name.value,
				std::move(typeValue),
				std::move(expressionValue));
	}

	advance();

	return { std::make_unique<StructExpressionNode>(token.getValue().position, std::move(fields)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseMatchExpression() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> value = parsePipeExpression();
	if(value.hasError()) return { std::move(value), Trace(ASTNodeType::MatchExpression, pos) };
	if(peek().type != Token::MatchArrow) return value;
	
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::MatchExpression, token.getValue().position) };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after match expression");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::MatchExpression,leftBrace.getValue().position) };

	std::vector<std::unique_ptr<MatchBranch>> branches;
	while(peek().type != Token::RightBrace) {

		Token::Position patternPos = peek().position;
		Result<std::unique_ptr<PatternNode>> parsedPattern = parsePattern();
		if(parsedPattern.hasError()) return { std::move(parsedPattern), Trace(ASTNodeType::MatchExpression, patternPos) };
		
		// Parse optional guard condition (&&)
		std::unique_ptr<ExpressionNode> condition = nullptr;
		if(peek().type == Token::LogicalAnd) {
			advance(); // consume &&
			Result<std::unique_ptr<ASTNode>> guardExpr = parseExpression();
			if(guardExpr.hasError()) return { std::move(guardExpr), Trace(ASTNodeType::MatchExpression, peek().position) };
			condition = dynamic_unique_cast<ExpressionNode>(guardExpr.moveValue());
		}

		Result<Token> arrow = expect(Token::Arrow, "Expected '->' after pattern");
		if(arrow.hasError()) return { arrow, Trace(ASTNodeType::MatchExpression, arrow.getValue().position) };

		Result<std::unique_ptr<ASTNode>> body = parseExpression();
		Token::Position bodyPos = peek().position;
		if(body.hasError()) return { std::move(body), Trace(ASTNodeType::MatchExpression, bodyPos) };
		
		branches.push_back(std::make_unique<MatchBranch>(
				patternPos,
				parsedPattern.moveValue(),
				std::move(condition),
				dynamic_unique_cast<ExpressionNode>(body.moveValue())));

		if(peek().type == Token::RightBrace) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' or '}'");
		if(comma.hasError()) return { comma, Trace(ASTNodeType::MatchExpression, comma.getValue().position) };
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after match statement");
	if(rightBrace.hasError()) return { rightBrace, Trace(ASTNodeType::MatchExpression, rightBrace.getValue().position) };


	return { std::make_unique<MatchExpressionNode>(
			pos,
			dynamic_unique_cast<ExpressionNode>(value.moveValue()), std::move(branches)) };
}

bool Parser::isLambdaExpression() {
	if (peek().type != Token::LeftParen) return false;

	size_t startIndex = current;

	Result<Token> leftParen = advance();
	if (leftParen.hasError()) {
		current = startIndex;
		return false;
	}

	while (peek().type != Token::RightParen) {
		Result<Token> token = advance();
		if(token.hasError()) {
			current = startIndex;
			return false;
		}
	}

	Result<Token> rightParen = advance();
	if (rightParen.hasError()) {
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
	if (identifier.hasError()) {
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
		if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::LambdaExpression, leftParen.getValue().position) };
		startPos = leftParen.getValue().position;

		while(peek().type != Token::RightParen) {
			Token::Position argPos = peek().position;
			Result<std::unique_ptr<FunctionArgument>> arg = parseFunctionArgument();
			if(arg.hasError()) return { std::move(arg), Trace(ASTNodeType::LambdaExpression, argPos) };

			args.push_back(arg.moveValue());

			if(peek().type == Token::RightParen) break;

			Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
			if(comma.hasError()) return { comma, Trace(ASTNodeType::LambdaExpression, comma.getValue().position) };
		}

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::LambdaExpression, rightParen.getValue().position) };
	} else {
		// Handle x -> ... syntax (single parameter without parentheses)
		Result<Token> identifier = expect(Token::Identifier, "Expected parameter name");
		if(identifier.hasError()) return { identifier, Trace(ASTNodeType::LambdaExpression, identifier.getValue().position) };
		startPos = identifier.getValue().position;
		
		auto arg = std::make_unique<FunctionArgument>(identifier.getValue().position);
		arg->name = identifier.getValue().value;
		arg->type = nullptr;
		args.push_back(std::move(arg));
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->'");
	if(arrow.hasError()) return { arrow, Trace(ASTNodeType::LambdaExpression, arrow.getValue().position) };

	Result<std::unique_ptr<ASTNode>> body = parseExpression();
	if (body.hasError()) return { std::move(body), Trace(ASTNodeType::LambdaExpression, body.getValue()->position) };

	auto bodyPosition = body.getValue()->position;
			return {std::make_unique<LambdaExpressionNode>(
			startPos,
			std::move(args),
			std::make_unique<ReturnStatementNode>(bodyPosition,
												  dynamic_unique_cast<ExpressionNode>(body.moveValue()), false))
	};
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
	if(underscore.hasError()) return { underscore, Trace(ASTNodeType::WildcardPattern, underscore.getValue().position) };
	
	if(underscore.getValue().value != "_") {
		return { "Expected '_' for wildcard pattern", "", Trace(ASTNodeType::WildcardPattern, underscore.getValue().position) };
	}
	
	return { std::make_unique<WildcardPatternNode>(underscore.getValue().position) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseLiteralPattern() {
	Result<std::unique_ptr<ASTNode>> literal = parsePrimary();
	if(literal.hasError()) return { std::move(literal), Trace(ASTNodeType::LiteralPattern, peek().position) };
	
	// Check if this is a range pattern (literal to literal)
	if(peek().type == Token::KeywordTo) {
		current--; // Go back to re-parse as range
		return parseRangePattern();
	}
	
	Token::Position literalPos = literal.getValue()->position;
	return { std::make_unique<LiteralPatternNode>(
		literalPos,
		dynamic_unique_cast<ExpressionNode>(literal.moveValue())
	) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseIdentifierPattern() {
	Result<Token> identifier = expect(Token::Identifier, "Expected identifier");
	if(identifier.hasError()) return { identifier, Trace(ASTNodeType::IdentifierPattern, identifier.getValue().position) };
	
	return { std::make_unique<IdentifierPatternNode>(
		identifier.getValue().position,
		identifier.getValue().value
	) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseArrayPattern() {
	Token::Position pos = peek().position;
	Result<Token> leftBracket = expect(Token::LeftBracket, "Expected '['");
	if(leftBracket.hasError()) return { leftBracket, Trace(ASTNodeType::ArrayPattern, leftBracket.getValue().position) };
	
	std::vector<std::unique_ptr<PatternNode>> elements;
	std::unique_ptr<PatternNode> rest = nullptr;
	
	while(peek().type != Token::RightBracket) {
		if(peek().type == Token::Ellipsis) {
			// Rest pattern: ...rest
			advance(); // consume ...
			if(peek().type == Token::Identifier) {
				Result<std::unique_ptr<PatternNode>> restPattern = parsePattern();
				if(restPattern.hasError()) return { std::move(restPattern), Trace(ASTNodeType::ArrayPattern, peek().position) };
				rest = restPattern.moveValue();
			}
			break;
		}
		
		Result<std::unique_ptr<PatternNode>> element = parsePattern();
		if(element.hasError()) return { std::move(element), Trace(ASTNodeType::ArrayPattern, peek().position) };
		elements.push_back(element.moveValue());
		
		if(peek().type == Token::RightBracket) break;
		
		Result<Token> comma = expect(Token::Comma, "Expected ',' or ']'");
		if(comma.hasError()) return { comma, Trace(ASTNodeType::ArrayPattern, comma.getValue().position) };
	}
	
	Result<Token> rightBracket = expect(Token::RightBracket, "Expected ']'");
	if(rightBracket.hasError()) return { rightBracket, Trace(ASTNodeType::ArrayPattern, rightBracket.getValue().position) };
	
	return { std::make_unique<ArrayPatternNode>(pos, std::move(elements), std::move(rest)) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseStructPattern() {
	Token::Position pos = peek().position;
	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{'");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::StructPattern, leftBrace.getValue().position) };
	
	std::vector<std::pair<std::string, std::unique_ptr<PatternNode>>> fields;
	
	while(peek().type != Token::RightBrace) {
		Result<Token> fieldName = expect(Token::Identifier, "Expected field name");
		if(fieldName.hasError()) return { fieldName, Trace(ASTNodeType::StructPattern, fieldName.getValue().position) };
		
		std::unique_ptr<PatternNode> pattern;
		if(peek().type == Token::Colon) {
			advance(); // consume :
			Result<std::unique_ptr<PatternNode>> fieldPattern = parsePattern();
			if(fieldPattern.hasError()) return { std::move(fieldPattern), Trace(ASTNodeType::StructPattern, peek().position) };
			pattern = fieldPattern.moveValue();
		} else {
			// Shorthand: {x} is equivalent to {x: x}
			pattern = std::make_unique<IdentifierPatternNode>(fieldName.getValue().position, fieldName.getValue().value);
		}
		
		fields.emplace_back(fieldName.getValue().value, std::move(pattern));
		
		if(peek().type == Token::RightBrace) break;
		
		Result<Token> comma = expect(Token::Comma, "Expected ',' or '}'");
		if(comma.hasError()) return { comma, Trace(ASTNodeType::StructPattern, comma.getValue().position) };
	}
	
	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}'");
	if(rightBrace.hasError()) return { rightBrace, Trace(ASTNodeType::StructPattern, rightBrace.getValue().position) };
	
	return { std::make_unique<StructPatternNode>(pos, std::move(fields)) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseConstructorPattern() {
	Token::Position pos = peek().position;
	Result<Token> name = expect(Token::Identifier, "Expected constructor name");
	if(name.hasError()) return { name, Trace(ASTNodeType::ConstructorPattern, name.getValue().position) };
	
	std::string constructorName = name.getValue().value;
	
	// Handle enum constructors: Shape::Circle
	if(peek().type == Token::DoubleColon) {
		advance(); // consume ::
		Result<Token> enumName = expect(Token::Identifier, "Expected enum variant name");
		if(enumName.hasError()) return { enumName, Trace(ASTNodeType::ConstructorPattern, enumName.getValue().position) };
		constructorName += "::" + enumName.getValue().value;
	}
	
	std::vector<std::unique_ptr<PatternNode>> arguments;
	
	if(peek().type == Token::LeftParen) {
		advance(); // consume (
		
		while(peek().type != Token::RightParen) {
			Result<std::unique_ptr<PatternNode>> arg = parsePattern();
			if(arg.hasError()) return { std::move(arg), Trace(ASTNodeType::ConstructorPattern, peek().position) };
			arguments.push_back(arg.moveValue());
			
			if(peek().type == Token::RightParen) break;
			
			Result<Token> comma = expect(Token::Comma, "Expected ',' or ')'");
			if(comma.hasError()) return { comma, Trace(ASTNodeType::ConstructorPattern, comma.getValue().position) };
		}
		
		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::ConstructorPattern, rightParen.getValue().position) };
	}
	
	return { std::make_unique<ConstructorPatternNode>(pos, constructorName, std::move(arguments)) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseTypePattern() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> type = parseType();
	if(type.hasError()) return { std::move(type), Trace(ASTNodeType::TypePattern, pos) };
	
	return { std::make_unique<TypePatternNode>(pos, type.moveValue()) };
}

Result<std::unique_ptr<PatternNode>> Parser::parseRangePattern() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> start = parsePrimary();
	if(start.hasError()) return { std::move(start), Trace(ASTNodeType::RangePattern, pos) };
	
	bool isInclusive = false;
	if(peek().type == Token::KeywordTo) {
		advance(); // consume 'to'
		if(peek().type == Token::Assign) {
			advance(); // consume '='
			isInclusive = true;
		}
	} else {
		return { "Expected 'to' in range pattern", "", Trace(ASTNodeType::RangePattern, peek().position) };
	}
	
	Result<std::unique_ptr<ASTNode>> end = parsePrimary();
	if(end.hasError()) return { std::move(end), Trace(ASTNodeType::RangePattern, peek().position) };
	
	return { std::make_unique<RangePatternNode>(
		pos,
		dynamic_unique_cast<ExpressionNode>(start.moveValue()),
		dynamic_unique_cast<ExpressionNode>(end.moveValue()),
		isInclusive
	) };
}

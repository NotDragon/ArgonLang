#include "backend/Parser.h"
#include <stdexcept>

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
	} else if (token.type == Token::BooleanLiteral) {
		return { std::make_unique<BooleanLiteralNode>(token.position, token.value == "true") };
	} else if (token.type == Token::Identifier) {
		return { std::make_unique<IdentifierNode>(token.position, token.value) };
	}else if (token.type == Token::LeftParen) {
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

	while(peek().type == Token::Equal) {
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
	Result<std::unique_ptr<ASTNode>> left = parseParallelExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (assign.hasError()) return { assign, Trace(ASTNodeType::AssignmentExpression, assign.getValue().position) };

		Token::Position pos = peek().position;
		Result<std::unique_ptr<ASTNode>> right = parseParallelExpression();
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

	while(peek().type == Token::ReduceRange) {
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
	Result<std::unique_ptr<ASTNode>> left = parseIndexExpression();
	if(left.hasError()) return left;

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

Result<std::unique_ptr<ASTNode>> Parser::parseReferenceExpression() {
	if(peek().type != Token::MapRange) return parseDerefExpression();

	Result<Token> refToken = advance();
	if (refToken.hasError()) return {refToken, Trace(ASTNodeType::UnaryExpression, refToken.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> operand = parseDerefExpression();
	if (operand.hasError()) return { std::move(operand), Trace(ASTNodeType::UnaryExpression, pos) };

	return { std::make_unique<UnaryExpressionNode>(
			refToken.getValue().position,
			refToken.getValue(),
			dynamic_unique_cast<ExpressionNode>(operand.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseIncrementExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseReferenceExpression();
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
	Result<std::unique_ptr<ASTNode>> left = parseUnaryPlusExpression();
	if(left.hasError()) return left;


	while(peek().type == Token::LogicalNot) {
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
	if(peek().type != Token::KeywordPar) return parseLambdaExpression();

	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::UnaryExpression, token.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> left = parseStatement();
	if(left.hasError()) return { std::move(left), Trace(ASTNodeType::ParallelExpression, pos) };
	Token::Position leftPos = left.getValue()->position;
	return { std::make_unique<ParallelExpressionNode>(
			leftPos,
			left.moveValue())
	};
}

Result<std::unique_ptr<ASTNode>> Parser::parseStructExpression() {
	if(peek().type != Token::KeywordStruct) return parseMatchExpression();

	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::StructExpression, token.getValue().position) };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after struct");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::StructExpression, leftBrace.getValue().position) };

	std::vector<StructField> fields;
	while(peek().type != Token::RightBrace) {
		Result<Token> nameError = expect(Token::Identifier, "Expected fields base");
		if(nameError.hasError()) return { nameError, Trace(ASTNodeType::StructExpression, nameError.getValue().position) };
		Token name = nameError.getValue();

		Result<std::unique_ptr<TypeNode>> type;
		Result<std::unique_ptr<ASTNode>> expression;

		if(peek().type == Token::Colon) {
			Result<Token> token1 = advance();
			if(token1.hasError()) return { token1, Trace(ASTNodeType::StructExpression, token1.getValue().position) };

			Token::Position pos = peek().position;
			type = parseType();
			if(type.hasError()) return { std::move(type), Trace(ASTNodeType::StructExpression, pos) };
		}

		if(peek().type == Token::Equal) {
			Result<Token> token1 = advance();
			if(token1.hasError()) return { token1, Trace(ASTNodeType::StructExpression, token1.getValue().position) };

			Token::Position pos = peek().position;
			expression = parseExpression();
			if(expression.hasError()) return { std::move(expression), Trace(ASTNodeType::StructExpression, pos) };
		}

		if(type.isNull() && expression.isNull()) {
			return { "Cannot have field without value or type", "", Trace(ASTNodeType::StructExpression, peek().position) };
		}

		std::unique_ptr<TypeNode> typeValue = !type.isNull() ? type.moveValue() : nullptr;
		std::unique_ptr<ExpressionNode> expressionValue = !expression.isNull() ? dynamic_unique_cast<ExpressionNode>(expression.moveValue()) : nullptr;

		fields.emplace_back(
				name.position,
				name.value,
				std::move(typeValue),
				std::move(expressionValue));
	}

	return { std::make_unique<StructExpressionNode>(token.getValue().position, std::move(fields)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseMatchExpression() {
	Token::Position pos = peek().position;
	Result<std::unique_ptr<ASTNode>> value = parseFilterExpression();
	if(value.hasError()) return { std::move(value), Trace(ASTNodeType::MatchExpression, pos) };
	if(peek().type != Token::MatchArrow) return value;
	
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::MatchExpression, token.getValue().position) };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after match expression");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::MatchExpression,leftBrace.getValue().position) };

	std::vector<std::unique_ptr<MatchBranch>> branches;
	while(peek().type != Token::RightBrace) {

		Token::Position patternPos = peek().position;
		Result<std::unique_ptr<ASTNode>> pattern = parseExpression();
		if(pattern.hasError()) return { std::move(pattern), Trace(ASTNodeType::MatchExpression, patternPos) };

		Result<Token> arrow = expect(Token::Arrow, "Expected '->' after pattern");
		if(arrow.hasError()) return { arrow, Trace(ASTNodeType::MatchExpression, arrow.getValue().position) };

		Result<std::unique_ptr<ASTNode>> body = parseExpression();

		Token::Position bodyPos = peek().position;
		if(body.hasError()) return { std::move(body), Trace(ASTNodeType::MatchExpression, bodyPos) };

		branches.push_back(std::make_unique<MatchBranch>(
				patternPos,
				dynamic_unique_cast<ExpressionNode>(pattern.moveValue()),
				nullptr,
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


Result<std::unique_ptr<ASTNode>> Parser::parseLambdaExpression() {
	if(!isLambdaExpression()) return parseStructExpression();
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '('");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::LambdaExpression, leftParen.getValue().position) };
	std::vector<std::unique_ptr<FunctionArgument>> args;

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

	Result<Token> arrow = expect(Token::Arrow, "Expected '->'");
	if(arrow.hasError()) return { arrow, Trace(ASTNodeType::LambdaExpression, arrow.getValue().position) };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (body.hasError()) return { std::move(body), Trace(ASTNodeType::LambdaExpression, body.getValue()->position) };

	if(body.getValue()->getNodeGroup() == ASTNodeGroup::Expression) {
		auto bodyPosition = body.getValue()->position;
		return {std::make_unique<LambdaExpressionNode>(
				leftParen.getValue().position,
				std::move(args),
				std::make_unique<ReturnStatementNode>(bodyPosition,
													  dynamic_unique_cast<ExpressionNode>(body.moveValue()), false))
		};
	}

	return { std::make_unique<LambdaExpressionNode>(
			leftParen.getValue().position,
			std::move(args),
			body.moveValue())
	};
}

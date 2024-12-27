#include "backend/Parser.h"
#include <stdexcept>

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parsePrimary() {
	Result<Token> tokenError = advance();
	if(tokenError.hasError()) return { tokenError.getErrorMsg() };
	Token token = tokenError.getValue();

	if (token.type == Token::IntegralLiteral) {
		return { std::make_unique<IntegralLiteralNode>(std::stoi(token.value), determineIntegerType(token.value)) }; // stoi needs to be changed
	} else if (token.type == Token::FloatLiteral) {
		return { std::make_unique<FloatLiteralNode>(std::stod(token.value), determineFloatType(token.value)) }; // stod needs to be changed
	} else if (token.type == Token::StringLiteral) {
		return { std::make_unique<StringLiteralNode>(token.value) };
	} else if (token.type == Token::BooleanLiteral) {
		return { std::make_unique<BooleanLiteralNode>(token.value == "true") };
	} else if (token.type == Token::Identifier) {
		return { std::make_unique<IdentifierNode>(token.value) };
	} else if (token.type == Token::LeftParen) {
		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		Result<Token> tokenError1 = expect(Token::RightParen, "Expected closing ')'");
		if(tokenError1.hasError()) return { tokenError1.getErrorMsg() };

		return expr;
	}

	return { "Unexpected token: " + token.value + " " + Token::getTypeAsString(token.type) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseAdditiveExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMultiplicativeExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Plus || peek().type == Token::Minus) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseMultiplicativeExpression();
		if(right.hasError()) return right;
		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseBitwiseNotExpression();
		if(right.hasError()) return right;
		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseLogicalAndExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseEqualityExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseRelationalExpression();

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseBitwiseExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseShiftExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseToExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseAssignmentExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseParallelExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseMemberAccessExpression() {
	Result<std::unique_ptr<ASTNode>> left = parsePrimary();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseIndexingExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMemberAccessExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseFilterExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMapExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::FilterRange) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseMapExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseReduceExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		Result<std::unique_ptr<ASTNode>> right = parseLogicalOrExpression();
		if(right.hasError()) return right;

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				op,
				dynamic_unique_cast<ExpressionNode>(right.moveValue())
		);
	}

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseRangeExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseFunctionCallExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionCallExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseIndexingExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::LeftParen) {
		Result<Token> leftParen = advance();
		if(leftParen.hasError()) return { leftParen.getErrorMsg() };

		std::vector<std::unique_ptr<ExpressionNode>> arguments;
		while(peek().type != Token::RightParen) {
			Result<std::unique_ptr<ASTNode>> argument = parseExpression();
			if(argument.hasError()) return argument;

			arguments.push_back(dynamic_unique_cast<ExpressionNode>(argument.moveValue()));
			if(peek().type == Token::RightParen) break;

			Result<Token> comma = expect(Token::Comma, "Expected ',' between function arguments");
			if(comma.hasError()) return { comma.getErrorMsg() };
		}
		Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function arguments");
		if(rightParen.hasError()) return { rightParen.getErrorMsg() };

		left = std::make_unique<FunctionCallExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		bool isInclusive = op.type == Token::Assign;
		Result<std::unique_ptr<ASTNode>> right = parseAdditiveExpression();
		if(right.hasError()) return right;

		left = std::make_unique<ToExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(left.moveValue()),
				dynamic_unique_cast<ExpressionNode>(right.moveValue()),
				isInclusive
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseDerefExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseRangeExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseReferenceExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseDerefExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseIncrementExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseReferenceExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Increment) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		left = std::make_unique<UnaryExpressionNode>(
				op,
				dynamic_unique_cast<ExpressionNode>(left.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnaryMinusExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseIncrementExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnaryPlusExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseUnaryMinusExpression();

	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalNotExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseUnaryPlusExpression();
	if(left.hasError()) return left;


	while(peek().type == Token::LogicalNot) {
		Result<Token> opError = advance();
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		left = std::make_unique<UnaryExpressionNode>(
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
		if(opError.hasError()) return { opError.getErrorMsg() };
		Token op = opError.getValue();

		left = std::make_unique<UnaryExpressionNode>(
				op,
				dynamic_unique_cast<ExpressionNode>(left.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseParallelExpression() {
	if(peek().type != Token::KeywordPar) return parseStructExpression();

	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> left = parseStatement();
	if(left.hasError()) return left;

	return { std::make_unique<ParallelExpressionNode>(left.moveValue()) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseStructExpression() {
	if(peek().type != Token::KeywordStruct) return parseMatchExpression();

	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after struct");
	if(leftBrace.hasError()) return { leftBrace.getErrorMsg() };

	std::vector<StructField> fields;
	while(peek().type != Token::RightBrace) {
		Result<Token> nameError = expect(Token::Identifier, "Expected fields name");
		if(nameError.hasError()) return { nameError.getErrorMsg() };
		Token name = nameError.getValue();

		Result<std::unique_ptr<TypeNode>> type;
		Result<std::unique_ptr<ASTNode>> expression;

		if(peek().type == Token::Colon) {
			Result<Token> token1 = advance();
			if(token1.hasError()) return { token1.getErrorMsg() };

			type = parseType();
			if(type.hasError()) return { type.getErrorMsg() };
		}

		if(peek().type == Token::Equal) {
			Result<Token> token1 = advance();
			if(token1.hasError()) return { token1.getErrorMsg() };

			expression = parseExpression();
			if(expression.hasError()) return expression;
		}

		if(type.isNull() && expression.isNull()) {
			return { "Cannot have field without value or type" };
		}

		std::unique_ptr<TypeNode> typeValue = !type.isNull() ? type.moveValue() : nullptr;
		std::unique_ptr<ExpressionNode> expressionValue = !expression.isNull() ? dynamic_unique_cast<ExpressionNode>(expression.moveValue()) : nullptr;

		fields.emplace_back(name.value, std::move(typeValue), std::move(expressionValue));
	}

	return { std::make_unique<StructExpressionNode>(std::move(fields)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseMatchExpression() {
	Result<std::unique_ptr<ASTNode>> value = parseFilterExpression();
	if(value.hasError()) return value;
	if(peek().type != Token::MatchArrow) return value;
	
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after match expression");
	if(leftBrace.hasError()) return { leftBrace.getErrorMsg() };

	std::vector<std::unique_ptr<MatchBranch>> branches;
	while(peek().type != Token::RightBrace) {

		Result<std::unique_ptr<ASTNode>> pattern = parseExpression();
		if(pattern.hasError()) return pattern;

		Result<Token> arrow = expect(Token::Arrow, "Expected '->' after pattern");
		if(arrow.hasError()) return { arrow.getErrorMsg() };

		Result<std::unique_ptr<ASTNode>> body = parseExpression();

		branches.push_back(std::make_unique<MatchBranch>(dynamic_unique_cast<ExpressionNode>(pattern.moveValue()), nullptr, dynamic_unique_cast<ExpressionNode>(body.moveValue())));
		if(peek().type == Token::RightBrace) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' or '}'");
		if(comma.hasError()) return { comma.getErrorMsg() };
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after match statement");
	if(rightBrace.hasError()) return { rightBrace.getErrorMsg() };

	return { std::make_unique<MatchExpressionNode>(dynamic_unique_cast<ExpressionNode>(value.moveValue()), std::move(branches)) };
}

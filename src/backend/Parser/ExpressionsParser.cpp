#include "backend/Parser.h"
#include <stdexcept>

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parsePrimary() {
	const Token& token = advance();

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
		auto expr = parseExpression();
		expect(Token::RightParen, "Expected closing ')'");
		return expr;
	}

	return { "Unexpected token: " + token.value + " " + Token::getTypeAsString(token.type) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseAdditiveExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseMultiplicativeExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::Plus || peek().type == Token::Minus) {
		const Token op = advance();
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
		const Token op = advance();
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

std::unique_ptr<ProgramNode> Parser::parse() {
	std::vector<std::unique_ptr<ASTNode>> statements;
	while(!eos()) {
		Result<std::unique_ptr<ASTNode>> statement = parseStatement();
		if(statement.hasError()) throw std::runtime_error(statement.getErrorMsg());

		statements.push_back(statement.moveValue());
	}
	return std::make_unique<ProgramNode>(std::move(statements));
}

Parser::Parser(const std::vector<Token> &tokens)  : tokens(tokens) {}

Result<std::unique_ptr<ASTNode>> Parser::parseLogicalOrExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalAndExpression();
	if(left.hasError()) return left;

	while(peek().type == Token::LogicalOr) {
		const Token op = advance();
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
		const Token op = advance();
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
		const Token op = advance();
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

	while(peek().type == Token::Greater || peek().type == Token::GreaterEqual || peek().type == Token::Less || peek().type == Token::LessEqual) {
		const Token op = advance();
		Result<std::unique_ptr<ASTNode>> right = parseBitwiseExpression();

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

	while(peek().type == Token::BitwiseOr || peek().type == Token::BitwiseAnd || peek().type == Token::BitwiseXor) {
		const Token op = advance();
		Result<std::unique_ptr<ASTNode>> right = parseShiftExpression();

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

	while(peek().type == Token::LeftShift || peek().type == Token::RightShift) {
		const Token op = advance();
		Result<std::unique_ptr<ASTNode>> right = parseToExpression();

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

	while(peek().type == Token::FilterRange) {
		const Token op = advance();
		Result<std::unique_ptr<ASTNode>> right = parseMapExpression();

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

	while(peek().type == Token::MapRange) {
		const Token op = advance();
		Result<std::unique_ptr<ASTNode>> right = parseReduceExpression();

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

	while(peek().type == Token::ReduceRange) {
		const Token op = advance();

		Result<std::unique_ptr<ASTNode>> right = parseLogicalOrExpression();

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

	return {left.moveValue() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionCallExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseIndexingExpression();

	return {left.moveValue() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseToExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseAdditiveExpression();

	while(peek().type == Token::KeywordTo) {
		bool isInclusive = advance().type == Token::Assign;
		Result<std::unique_ptr<ASTNode>> right = parseAdditiveExpression();

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

	while(peek().type == Token::Increment) {
		left = std::make_unique<UnaryExpressionNode>(
				advance(),
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

	while(peek().type == Token::LogicalNot) {
		left = std::make_unique<UnaryExpressionNode>(
				advance(),
				dynamic_unique_cast<ExpressionNode>(left.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseBitwiseNotExpression() {
	Result<std::unique_ptr<ASTNode>> left = parseLogicalNotExpression();

	while(peek().type == Token::BitwiseNot) {
		left = std::make_unique<UnaryExpressionNode>(
				advance(),
				dynamic_unique_cast<ExpressionNode>(left.moveValue())
		);
	}
	return left;
}

Result<std::unique_ptr<ASTNode>> Parser::parseParallelExpression() {
	if(peek().type != Token::KeywordParallel) return parseAwaitExpression();
	advance();
	Result<std::unique_ptr<ASTNode>> left = parseStatement();

	return { std::make_unique<ParallelExpressionNode>(left.moveValue()) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseAwaitExpression() {
	if(peek().type != Token::KeywordAwait) return parseLazyExpression();
	advance();
	Result<std::unique_ptr<ASTNode>> left = parseStatement();

	return { std::make_unique<AwaitExpressionNode>(dynamic_unique_cast<ExpressionNode>(left.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseLazyExpression() {
	if(peek().type != Token::KeywordLazy) return parseStructExpression();
	advance();
	Result<std::unique_ptr<ASTNode>> left = parseStatement();

	return { std::make_unique<LazyExpressionNode>(dynamic_unique_cast<ExpressionNode>(left.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseStructExpression() {
	if(peek().type != Token::KeywordStruct) return parseMatchExpression();
	advance();
	expect(Token::LeftBrace, "Expected '{' after struct");

	std::vector<StructField> fields;
	while(peek().type != Token::RightBrace) {
		Token name = expect(Token::Identifier, "Expected fields name");
		Result<std::unique_ptr<TypeNode>> type;
		Result<std::unique_ptr<ExpressionNode>> expression;
		if(peek().type == Token::Colon) {
			advance();
			type = parseType();
		}
		if(peek().type == Token::Equal) {
			advance();
			expression = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
		}
		if(!type.hasValue() && !expression.hasValue()) {
			return { "Cannot have field without value or type" };
		}

		std::unique_ptr<TypeNode> typeValue = type.hasValue() ? type.moveValue() : nullptr;
		std::unique_ptr<ExpressionNode> expressionValue = expression.hasValue() ? std::move(expression.moveValue()) : nullptr;

		fields.emplace_back(name.value, std::move(typeValue), std::move(expressionValue));
	}

	return { std::make_unique<StructExpressionNode>(std::move(fields)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseMatchExpression() {
	if(peek().type != Token::KeywordMatch) return parseFilterExpression();
	advance();
	expect(Token::LeftParen, "Expected '(' after match");
	std::unique_ptr<ExpressionNode> value = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
	expect(Token::RightParen, "Expected ')' after identifier");
	expect(Token::LeftBrace, "Expected '{' after match expression");
	std::vector<std::unique_ptr<MatchBranch>> branches;
	while(peek().type != Token::RightBrace) {
		std::unique_ptr<ExpressionNode> pattern = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
		expect(Token::Arrow, "Expected '->' after pattern");
		std::unique_ptr<ExpressionNode> body = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
		branches.push_back(std::make_unique<MatchBranch>(std::move(pattern), nullptr, std::move(body)));
		if(peek().type == Token::RightBrace) break;
		expect(Token::Comma, "Expected ',' or '}'");
	}
	expect(Token::RightBrace, "Expected '}' after match statement");

	return { std::make_unique<MatchExpressionNode>(std::move(value), std::move(branches)) };
}
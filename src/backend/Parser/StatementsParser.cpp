#include "backend/Parser.h"
#include "Error/Result.h"
#include <stdexcept>

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parseStatement() {
	switch (peek().type) {
//		case Token::KeywordUnion:
//			break;
//		case Token::KeywordEval:
//			break;
//		case Token::KeywordType:
//			break;
		case Token::KeywordDef:
			return { parseVariableDeclaration() };
		case Token::KeywordIf:
			return { parseIfStatement() };
		case Token::KeywordWhile:
			return { parseWhileStatement() };
		case Token::KeywordFor:
			return { parseForStatement() };
		case Token::KeywordReturn:
			return { parseReturnStatement() };
		case Token::KeywordFunc:
			return { parseFunctionDeclaration() };
		case Token::KeywordClass:
			return { parseClassDeclaration() };
		case Token::KeywordImpl:
			return { parseImplStatement() };
		case Token::KeywordUsing:
			return { parseTypeAlias() };
		case Token::KeywordYield:
			return { parseYieldStatement() };
		case Token::KeywordWhen:
			return { parseWhenStatement() };
		case Token::KeywordParallel:
			return { parseParallelExpression() };
		case Token::KeywordBreak:
			return { parseBreakStatement() };
		case Token::KeywordContinue:
			return { parseContinueStatement() };
		case Token::LeftBrace:
			return { parseBlock() };
		default:
			return { parseExpression() };
	}
}

Result<std::unique_ptr<ASTNode>> Parser::parseVariableDeclaration() {
	Result<Token> keywordError = advance();
	if(keywordError.hasError()) return { keywordError.getErrorMsg() };
	Token keyword = keywordError.getValue();

	Result<Token> identifierError = expect(Token::Identifier, "Expected identifier after declaration");
	if(identifierError.hasError()) return { identifierError.getErrorMsg() };
	Token identifier = identifierError.getValue();

	Result<std::unique_ptr<TypeNode>> type;
	Result<std::unique_ptr<ASTNode>> value;

	if(peek().type == Token::Colon) {
		Result<Token> token = advance();
		if(token.hasError()) return { token.getErrorMsg() };

		type = parseType();
	}

	if(peek().type == Token::Assign) {
		Result<Token> token = advance();
		if(token.hasError()) return { token.getErrorMsg() };

		value = parseExpression();
		if(value.hasError()) return value;
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg() };

	if(type.isNull() && value.isNull()) {
//		return { "Expected either value or type, in variable declaration" };
	}

	return { std::make_unique<VariableDeclarationNode>((keyword.value == "const"), !type.isNull() ? type.moveValue() : nullptr,
													   !value.isNull() ? dynamic_unique_cast<ExpressionNode>(value.moveValue()) : nullptr,
													   std::move(identifier.value)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> identifier = expect(Token::Identifier, "Expected identifier after func declaration");
	if(identifier.hasError()) return { identifier.getErrorMsg() };

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after function declaration");
	if(leftParen.hasError()) return { leftParen.getErrorMsg() };

	std::vector<FunctionArgument> args;
	while(peek().type != Token::RightParen) {
		Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
		std::unique_ptr<TypeNode> type;
		std::unique_ptr<ExpressionNode> value;


		if(argIdentifier.hasError()) return { argIdentifier.getErrorMsg() };
		if(peek().type == Token::Colon) {
			Result<Token> colon = advance();
			if(colon.hasError()) return { colon.getErrorMsg() };

			Result<std::unique_ptr<TypeNode>> typeRes = parseType();
			if(typeRes.hasError()) return { typeRes.getErrorMsg() };

			type = typeRes.moveValue();
		}

		if(peek().type == Token::Assign) {
			Result<Token> assign = advance();
			if(assign.hasError()) return { assign.getErrorMsg() };

			Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
			if(valueRes.hasError()) return { valueRes.getErrorMsg() };

			value = dynamic_unique_cast<ExpressionNode>(valueRes.moveValue());
		}
		args.emplace_back(std::move(type), std::move(value), std::move(argIdentifier.getValue().value));
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function declaration");
	if(rightParen.hasError()) return { rightParen.getErrorMsg() };

	Result<std::unique_ptr<TypeNode>> returnType = parseType();
	if(returnType.hasError()) return { returnType.getErrorMsg() };

	if(peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if(semicolon.hasError()) return { semicolon.getErrorMsg() };
		return { std::make_unique<FunctionDefinitionNode>(returnType.moveValue(), std::move(args), std::move(identifier.getValue().value)) };
	}

	if(peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if(arrow.hasError()) return { arrow.getErrorMsg() };

		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		if(expr.hasError()) return expr;

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue()));
		return { std::make_unique<FunctionDeclarationNode>(returnType.moveValue(), std::move(args), std::move(body), std::move(identifier.getValue().value)) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	return { std::make_unique<FunctionDeclarationNode>(returnType.moveValue(), std::move(args), body.moveValue(), std::move(identifier.getValue().value)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseIfStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if(leftParen.hasError()) return { leftParen.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if(condition.hasError()) return condition;

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if(rightParen.hasError()) return { rightParen.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token1 = advance();
		if(token1.hasError()) return { token1.getErrorMsg() };

		elseStatement = parseStatement();
	}
	return { std::make_unique<IfStatementNode>(dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
	        dynamic_unique_cast<StatementNode>(body.moveValue()),
			dynamic_unique_cast<StatementNode>(elseStatement.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseForStatement() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhileStatement() {
	Result<Token> keywordError = advance();
	if(keywordError.hasError()) return { keywordError.getErrorMsg() };
	Token keyword = keywordError.getValue();

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if(leftParen.hasError()) return { leftParen.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if(condition.hasError()) return condition;

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if(rightParen.hasError()) return { rightParen.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token = advance();
		if(token.hasError()) return { token.getErrorMsg() };

		elseStatement = parseStatement();
	}
	return { std::make_unique<WhileStatementNode>(keyword.value == "dowhile",
												  dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
												  dynamic_unique_cast<StatementNode>(body.moveValue()),
												  dynamic_unique_cast<StatementNode>(elseStatement.moveValue())) };
}


Result<std::unique_ptr<ASTNode>> Parser::parseReturnStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return expr;

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg() };

	return { std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBreakStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg() };

	return { std::make_unique<BreakStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseContinueStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg() };

	return { std::make_unique<ContinueStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhenStatement() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseTypeAlias() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseClassDeclaration() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseBlock() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		Result<std::unique_ptr<ASTNode>> statement = parseStatement();
		if(statement.hasError()) return statement;

		body.push_back(statement.moveValue());
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after opening '{'");
	if(rightBrace.hasError()) return { rightBrace.getErrorMsg() };

	return { std::make_unique<BlockNode>(std::move(body)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseImplStatement() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseYieldStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return expr;

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg() };

	return { std::make_unique<YieldStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue())) };
}
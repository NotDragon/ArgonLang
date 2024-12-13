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
	Token keyword = advance();
	Token identifier = expect(Token::Identifier, "Expected identifier after declaration");
	Result<std::unique_ptr<TypeNode>> type;
	Result<std::unique_ptr<ExpressionNode>> value;

	if(peek().type == Token::Colon) {
		advance();
		type = parseType();
	}

	if(peek().type == Token::Assign) {
		advance();
		value = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
	}

	expect(Token::Semicolon, "Expected ';'");

	if(!type.hasValue() && !value.hasValue()) {
//		return { "Expected either value or type, in variable declaration" };
	}
	auto i = std::make_unique<VariableDeclarationNode>((keyword.value == "const"), type.hasValue()? type.moveValue(): nullptr, value.hasValue()? value.moveValue(): nullptr,
													   std::move(identifier.value));
	return { std::move(i) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDefinition() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseIfStatement() {
	advance();
	expect(Token::LeftParen, "Expected '(' after if statement");
	std::unique_ptr<ExpressionNode> condition = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
	expect(Token::RightParen, "Expected ')' after condition");

	std::unique_ptr<StatementNode> body = dynamic_unique_cast<StatementNode>(parseStatement().moveValue());
	std::unique_ptr<StatementNode> elseStatement = nullptr;

	if(peek().type == Token::KeywordElse) {
		advance();
		elseStatement = dynamic_unique_cast<StatementNode>(parseStatement().moveValue());
	}
	return { std::make_unique<IfStatementNode>(std::move(condition), std::move(body), std::move(elseStatement)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseForStatement() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhileStatement() {
	Token keyword = advance();
	expect(Token::LeftParen, "Expected '(' after while statement");
	std::unique_ptr<ExpressionNode> condition = dynamic_unique_cast<ExpressionNode>(parseExpression().moveValue());
	expect(Token::RightParen, "Expected ')' after condition");

	std::unique_ptr<StatementNode> body = dynamic_unique_cast<StatementNode>(parseStatement().moveValue());
	std::unique_ptr<StatementNode> elseStatement = nullptr;

	if(peek().type == Token::KeywordElse) {
		advance();
		elseStatement = dynamic_unique_cast<StatementNode>(parseStatement().moveValue());
	}
	return { std::make_unique<WhileStatementNode>(keyword.value == "dowhile", std::move(condition), std::move(body),
												 std::move(elseStatement)) };
}


Result<std::unique_ptr<ASTNode>> Parser::parseReturnStatement() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseBreakStatement() {
	advance();
	expect(Token::Semicolon, "Expected ';'");
	return { std::make_unique<BreakStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseContinueStatement() {
	advance();
	expect(Token::Semicolon, "Expected ';'");
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
	advance();
	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		body.push_back(parseStatement().moveValue());
	}
	expect(Token::RightBrace, "Expected '}' after opening '{'");
	return { std::make_unique<BlockNode>(std::move(body)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseImplStatement() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseYieldStatement() {
	return std::unique_ptr<ASTNode>();
}
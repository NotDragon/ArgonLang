#include "backend/Parser.h"
#include <stdexcept>

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseStatement() {
	switch (peek().type) {
//		case Token::KeywordUnion:
//			break;
//		case Token::KeywordEval:
//			break;
//		case Token::KeywordType:
//			break;
		case Token::KeywordDef:
			return parseVariableDeclaration();
		case Token::KeywordIf:
			return parseIfStatement();
		case Token::KeywordWhile:
			return parseWhileStatement();
		case Token::KeywordFor:
			return parseForStatement();
		case Token::KeywordReturn:
			return parseReturnStatement();
		case Token::KeywordFunc:
			return parseFunctionDeclaration();
		case Token::KeywordClass:
			return parseClassDeclaration();
		case Token::KeywordImpl:
			return parseImplStatement();
		case Token::KeywordUsing:
			return parseTypeAlias();
		case Token::KeywordYield:
			return parseYieldStatement();
		case Token::KeywordWhen:
			return parseWhenStatement();
		case Token::KeywordParallel:
			return parseParallelExpression();
		case Token::KeywordBreak:
			return parseBreakStatement();
		case Token::KeywordContinue:
			return parseContinueStatement();
		case Token::LeftBrace:
			return parseBlock();
		default:
			return parseExpression();
	}
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseVariableDeclaration() {
	Token keyword = advance();
	Token identifier = expect(Token::Identifier, "Expected identifier after declaration");
	std::unique_ptr<TypeNode> type = nullptr;
	std::unique_ptr<ExpressionNode> value;

	if(peek().type == Token::Colon) {
		advance();
		type = parseType();
	}

	if(peek().type == Token::Assign) {
		advance();
		value = dynamic_unique_cast<ExpressionNode>(parseExpression());
	}

	expect(Token::Semicolon, "Expected ';'");

	if(!type && !value) {
		// error
	}

	return std::make_unique<VariableDeclarationNode>(keyword.value == "const", std::move(type), std::move(value), std::move(identifier.value));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseFunctionDeclaration() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseFunctionDefinition() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseIfStatement() {
	advance();
	expect(Token::LeftParen, "Expected '(' after if statement");
	std::unique_ptr<ExpressionNode> condition = dynamic_unique_cast<ExpressionNode>(parseExpression());
	expect(Token::RightParen, "Expected ')' after condition");

	std::unique_ptr<StatementNode> body = dynamic_unique_cast<StatementNode>(parseStatement());
	std::unique_ptr<StatementNode> elseStatement = nullptr;

	if(peek().type == Token::KeywordElse) {
		advance();
		elseStatement = dynamic_unique_cast<StatementNode>(parseStatement());
	}
	return std::make_unique<IfStatementNode>(std::move(condition), std::move(body), std::move(elseStatement));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseForStatement() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseWhileStatement() {
	Token keyword = advance();
	expect(Token::LeftParen, "Expected '(' after while statement");
	std::unique_ptr<ExpressionNode> condition = dynamic_unique_cast<ExpressionNode>(parseExpression());
	expect(Token::RightParen, "Expected ')' after condition");

	std::unique_ptr<StatementNode> body = dynamic_unique_cast<StatementNode>(parseStatement());
	std::unique_ptr<StatementNode> elseStatement = nullptr;

	if(peek().type == Token::KeywordElse) {
		advance();
		elseStatement = dynamic_unique_cast<StatementNode>(parseStatement());
	}
	return std::make_unique<WhileStatementNode>(keyword.value == "dowhile", std::move(condition), std::move(body), std::move(elseStatement));
}


std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseReturnStatement() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseBreakStatement() {
	advance();
	expect(Token::Semicolon, "Expected ';'");
	return std::make_unique<BreakStatementNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseContinueStatement() {
	advance();
	expect(Token::Semicolon, "Expected ';'");
	return std::make_unique<ContinueStatementNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseWhenStatement() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseTypeAlias() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseClassDeclaration() {
	return std::unique_ptr<ArgonLang::ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseBlock() {
	advance();
	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		body.push_back(parseStatement());
	}
	expect(Token::RightBrace, "Expected '}' after opening '{'");
	return std::make_unique<BlockNode>(std::move(body));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseImplStatement() {
	return std::unique_ptr<ASTNode>();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseYieldStatement() {
	return std::unique_ptr<ASTNode>();
}


std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseType() {
	advance();
	return std::unique_ptr<TypeNode>();
}
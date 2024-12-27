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
		case Token::KeywordConstructor:
			return { parseConstructorStatement() };
		case Token::KeywordImpl:
			return { parseImplStatement() };
		case Token::KeywordUsing:
			return { parseTypeAlias() };
		case Token::KeywordYield:
			return { parseYieldStatement() };
		case Token::KeywordPar:
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

Result<std::unique_ptr<FunctionArgument>> Parser::parseFunctionArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier.getErrorMsg() };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;

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

	return { std::make_unique<FunctionArgument>(std::move(type), std::move(value), std::move(argIdentifier.getValue().value)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> identifier = parseMemberAccessExpression();
	if(identifier.hasError()) return identifier;

	if(!identifier.isNull() && identifier.getValue()->getNodeType() == ASTNodeType::Identifier) {
		auto tempIdentifier = dynamic_cast<IdentifierNode*>(identifier.getValue().get());
		if(tempIdentifier->identifier == "main") mainCounter++;
	}

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after function declaration");
	if(leftParen.hasError()) return { leftParen.getErrorMsg() };

	std::vector<std::unique_ptr<FunctionArgument>> args;
	while(peek().type != Token::RightParen) {
		Result<std::unique_ptr<FunctionArgument>> arg = parseFunctionArgument();
		if(arg.hasError()) return { arg.getErrorMsg() };

		args.push_back(arg.moveValue());

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if(comma.hasError()) return { comma.getErrorMsg() };
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function declaration");
	if(rightParen.hasError()) return { rightParen.getErrorMsg() };

	Result<std::unique_ptr<TypeNode>> returnType;
	if(peek().type != Token::LeftBrace) {
		returnType = parseType();
		if (returnType.hasError()) return {returnType.getErrorMsg()};
	}

	if(peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if(semicolon.hasError()) return { semicolon.getErrorMsg() };
		return { std::make_unique<FunctionDefinitionNode>(returnType.moveValue(), std::move(args), dynamic_unique_cast<ExpressionNode>(identifier.moveValue())) };
	}

	if(peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if(arrow.hasError()) return { arrow.getErrorMsg() };

		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		if(expr.hasError()) return expr;

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue()));

		Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after inline function");
		if(semiColon.hasError()) return { semiColon.getErrorMsg() };

		return { std::make_unique<FunctionDeclarationNode>(returnType.moveValue(), std::move(args), std::move(body), dynamic_unique_cast<ExpressionNode>(identifier.moveValue())) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	return { std::make_unique<FunctionDeclarationNode>(
			returnType.isNull()? nullptr :returnType.moveValue(),
			std::move(args),
			body.moveValue(),
			dynamic_unique_cast<ExpressionNode>(identifier.moveValue()))
	};
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
	Result<Token> keyword = advance();
	if(keyword.hasError()) return { keyword.getErrorMsg() };
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after for statement");
	if(leftParen.hasError()) return { leftParen.getErrorMsg() };

	Result<Token> identifierRes = expect(Token::Identifier, "Expected identifier");
	if(identifierRes.hasError()) return { identifierRes.getErrorMsg() };

	std::unique_ptr<TypeNode> type;
	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon.getErrorMsg() };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { typeRes.getErrorMsg() };
		type = typeRes.moveValue();
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->' after variable");
	if(arrow.hasError()) return { arrow.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> iterator = parseExpression();
	if(iterator.hasError()) return iterator;

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after expression");
	if(rightParen.hasError()) return { rightParen.getErrorMsg() };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	return { std::make_unique<ForStatementNode>(
			identifierRes.getValue().value,
			dynamic_unique_cast<ExpressionNode>(iterator.moveValue()),
			dynamic_unique_cast<StatementNode>(body.moveValue()),
			std::move(type)
	) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhileStatement() {
	Result<Token> keywordRes = advance();
	if(keywordRes.hasError()) return {keywordRes.getErrorMsg() };
	Token keyword = keywordRes.getValue();

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

Result<std::unique_ptr<ASTNode>> Parser::parseTypeAlias() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseClassDeclaration() {
	Result<Token> classKeyword = advance();
	if(classKeyword.hasError()) return { classKeyword.getErrorMsg() };

	Result<Token> className = advance();
	if(className.hasError()) return { className.getErrorMsg() };

	std::vector<ConstructorStatementNode> constructors;
	std::vector<ClassDeclarationNode::ClassMember> members;

//  TODO: in the very far future
//	if(peek().type == Token::RightParen) {
//
//	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after class declaration");
	if(leftBrace.hasError()) return { leftBrace.getErrorMsg() };

	while(peek().type != Token::RightBrace) {
		MemberVisibility visibility = MemberVisibility::PRI;

		if(	peek().type == Token::KeywordPub ||
			peek().type == Token::KeywordPri ||
			peek().type == Token::KeywordPro) {
			Result<Token> visKeyword = advance();
			if(visKeyword.hasError()) return { visKeyword.getErrorMsg() };

			visibility = 	visKeyword.getValue().type == Token::KeywordPub ? MemberVisibility::PUB :
							visKeyword.getValue().type == Token::KeywordPri ? MemberVisibility::PRI :
							MemberVisibility::PRO;

		}

		if(peek().type != Token::KeywordDef &&
		   peek().type == Token::KeywordFunc &&
		   peek().type != Token::KeywordConstructor) return { "Expected either function declaration, variable declaration or constructor declaration" };

		Result<std::unique_ptr<ASTNode>> member = parseStatement();
		if(member.hasError()) return member;

		members.emplace_back(dynamic_unique_cast<StatementNode>(member.moveValue()), visibility);
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after class declaration");
	if(rightBrace.hasError()) return { rightBrace.getErrorMsg() };

	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> Parser::parseConstructorArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier.getErrorMsg() };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;
	std::string initializes;

	if(peek().type == Token::LeftParen) {
		Result<Token> leftParen = expect(Token::LeftParen, "Expected '('");
		if(leftParen.hasError()) return { leftParen.getErrorMsg() };

		Result<Token> initIdentifier = expect(Token::Identifier, "Expected identifier");
		if(initIdentifier.hasError()) return { initIdentifier.getErrorMsg() };
		initializes = initIdentifier.getValue().value;

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if(rightParen.hasError()) return { rightParen.getErrorMsg() };
	}

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

	return { std::make_unique<ConstructorStatementNode::ConstructorArgument>(argIdentifier.getValue().value, initializes, std::move(type), std::move(value)) };

}

Result<std::unique_ptr<ASTNode>> Parser::parseConstructorStatement() {
	Result<Token> constructorKeyword = advance();
	if(constructorKeyword.hasError()) return { constructorKeyword.getErrorMsg() };

	std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after constructor");
	if(leftParen.hasError()) return { leftParen.getErrorMsg() };

	while(true) {
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> arg = parseConstructorArgument();
		if(arg.hasError()) return { arg.getErrorMsg() };

		args.push_back(arg.moveValue());

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if(comma.hasError()) return { comma.getErrorMsg() };

	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after constructor");
	if(rightParen.hasError()) return { rightParen.getErrorMsg() };

	if(peek().type == Token::Semicolon) {
		return { std::make_unique<ConstructorStatementNode>(std::move(args), nullptr) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	return { std::make_unique<ConstructorStatementNode>(std::move(args), body.moveValue()) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBlock() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		Result<std::unique_ptr<ASTNode>> statement = parseStatement();
		if(statement.hasError()) return statement;

		if(statement.getValue()->getNodeType() == ASTNodeType::FunctionCallExpression) {
			Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
			if(semiColon.hasError()) return { semiColon.getErrorMsg() };
		}

		body.push_back(statement.moveValue());
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after opening '{'");
	if(rightBrace.hasError()) return { rightBrace.getErrorMsg() };

	return { std::make_unique<BlockNode>(std::move(body)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseImplStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg() };

	Result<Token> className = expect(Token::Identifier, "Expected Class Name");
	if(className.hasError()) return { className.getErrorMsg() };

	Result<Token> keyword = advance();
	if(keyword.hasError()) return { keyword.getErrorMsg() };

	MemberVisibility visibility = MemberVisibility::PRI;
	if(	keyword.getValue().type == Token::KeywordPub ||
		keyword.getValue().type == Token::KeywordPri ||
		keyword.getValue().type == Token::KeywordPro) {
		visibility = 	keyword.getValue().type == Token::KeywordPub ? MemberVisibility::PUB :
						keyword.getValue().type == Token::KeywordPri ? MemberVisibility::PRI :
						MemberVisibility::PRO;

		keyword = advance();
		if(keyword.hasError()) return { keyword.getErrorMsg() };
	}

	if(keyword.getValue().type != Token::KeywordFunc && keyword.getValue().type != Token::KeywordDef) return { "Expected variable or function declaration" };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return body;

	return { std::make_unique<ImplStatementNode>(className.getValue().value,
											   dynamic_unique_cast<StatementNode>(body.moveValue()), visibility) };
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
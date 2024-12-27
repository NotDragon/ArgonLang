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
	if(keywordError.hasError()) return { keywordError.getErrorMsg(), Trace("", ASTNodeType::VariableDeclaration) };
	Token keyword = keywordError.getValue();

	Result<Token> identifierError = expect(Token::Identifier, "Expected identifier after declaration");
	if(identifierError.hasError()) return { identifierError.getErrorMsg(), Trace("", ASTNodeType::VariableDeclaration) };
	Token identifier = identifierError.getValue();

	Result<std::unique_ptr<TypeNode>> type;
	Result<std::unique_ptr<ASTNode>> value;

	if(peek().type == Token::Colon) {
		Result<Token> token = advance();
		if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::VariableDeclaration) };

		type = parseType();
	}

	if(peek().type == Token::Assign) {
		Result<Token> token = advance();
		if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::VariableDeclaration) };

		value = parseExpression();
		if(value.hasError()) return { std::move(value), Trace("", ASTNodeType::VariableDeclaration) };
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg(), Trace("", ASTNodeType::VariableDeclaration) };

	if(type.isNull() && value.isNull()) {
//		return { "Expected either value or type, in variable declaration" };
	}

	return { std::make_unique<VariableDeclarationNode>((keyword.value == "const"), !type.isNull() ? type.moveValue() : nullptr,
													   !value.isNull() ? dynamic_unique_cast<ExpressionNode>(value.moveValue()) : nullptr,
													   std::move(identifier.value)) };
}

Result<std::unique_ptr<FunctionArgument>> Parser::parseFunctionArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier.getErrorMsg(), Trace("", ASTNodeType::FunctionArgument) };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon.getErrorMsg(), Trace("", ASTNodeType::FunctionArgument) };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { typeRes.getErrorMsg(), Trace("", ASTNodeType::FunctionArgument) };

		type = typeRes.moveValue();
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if(assign.hasError()) return { assign.getErrorMsg(), Trace("", ASTNodeType::FunctionArgument) };

		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if(valueRes.hasError()) return { std::move(valueRes), Trace("", ASTNodeType::FunctionArgument) };

		value = dynamic_unique_cast<ExpressionNode>(valueRes.moveValue());
	}

	return { std::make_unique<FunctionArgument>(std::move(type), std::move(value), std::move(argIdentifier.getValue().value)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::FunctionDefinition) };

	Result<std::unique_ptr<ASTNode>> identifier = parseMemberAccessExpression();
	if(identifier.hasError()) return { std::move(identifier), Trace("", ASTNodeType::FunctionDefinition) };

	if(!identifier.isNull() && identifier.getValue()->getNodeType() == ASTNodeType::Identifier) {
		auto tempIdentifier = dynamic_cast<IdentifierNode*>(identifier.getValue().get());
		if(tempIdentifier->identifier == "main") mainCounter++;
	}

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after function declaration");
	if(leftParen.hasError()) return { leftParen.getErrorMsg(), Trace("", ASTNodeType::FunctionDefinition) };

	std::vector<std::unique_ptr<FunctionArgument>> args;
	while(peek().type != Token::RightParen) {
		Result<std::unique_ptr<FunctionArgument>> arg = parseFunctionArgument();
		if(arg.hasError()) return { std::move(arg), Trace("", ASTNodeType::FunctionDefinition) };

		args.push_back(arg.moveValue());

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if(comma.hasError()) return { comma.getErrorMsg(), Trace("", ASTNodeType::FunctionDefinition) };
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function declaration");
	if(rightParen.hasError()) return { rightParen.getErrorMsg(), Trace("", ASTNodeType::FunctionDefinition) };

	Result<std::unique_ptr<TypeNode>> returnType;
	if(peek().type != Token::LeftBrace) {
		returnType = parseType();
		if (returnType.hasError()) return { std::move(returnType), Trace("", ASTNodeType::FunctionDefinition) };
	}

	if(peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if(semicolon.hasError()) return { semicolon.getErrorMsg(), Trace("", ASTNodeType::FunctionDefinition) };
		return { std::make_unique<FunctionDefinitionNode>(returnType.moveValue(), std::move(args), dynamic_unique_cast<ExpressionNode>(identifier.moveValue())) };
	}

	if(peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if(arrow.hasError()) return { arrow.getErrorMsg(), Trace("", ASTNodeType::FunctionDeclaration) };

		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		if(expr.hasError()) return { std::move(expr), Trace("", ASTNodeType::FunctionDeclaration) };

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue()));

		Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after inline function");
		if(semiColon.hasError()) return { semiColon.getErrorMsg(), Trace("", ASTNodeType::FunctionDeclaration) };

		return { std::make_unique<FunctionDeclarationNode>(returnType.moveValue(), std::move(args), std::move(body), dynamic_unique_cast<ExpressionNode>(identifier.moveValue())) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace("", ASTNodeType::FunctionDeclaration) };

	return { std::make_unique<FunctionDeclarationNode>(
			returnType.isNull()? nullptr :returnType.moveValue(),
			std::move(args),
			body.moveValue(),
			dynamic_unique_cast<ExpressionNode>(identifier.moveValue()))
	};
}

Result<std::unique_ptr<ASTNode>> Parser::parseIfStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::IfStatement) };

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if(leftParen.hasError()) return { leftParen.getErrorMsg(), Trace("", ASTNodeType::IfStatement) };

	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if(condition.hasError()) return { std::move(condition), Trace("", ASTNodeType::IfStatement) };

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if(rightParen.hasError()) return { rightParen.getErrorMsg(), Trace("", ASTNodeType::IfStatement) };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace("", ASTNodeType::IfStatement) };

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token1 = advance();
		if(token1.hasError()) return { token1.getErrorMsg(), Trace("", ASTNodeType::IfStatement) };

		elseStatement = parseStatement();
		if(elseStatement.hasError()) return { std::move(elseStatement), Trace("", ASTNodeType::IfStatement) };
	}
	return { std::make_unique<IfStatementNode>(dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
	        dynamic_unique_cast<StatementNode>(body.moveValue()),
			dynamic_unique_cast<StatementNode>(elseStatement.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseForStatement() {
	Result<Token> keyword = advance();
	if(keyword.hasError()) return { keyword.getErrorMsg() };
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after for statement");
	if(leftParen.hasError()) return { leftParen.getErrorMsg(), Trace("", ASTNodeType::ForStatement) };

	Result<Token> identifierRes = expect(Token::Identifier, "Expected identifier");
	if(identifierRes.hasError()) return { identifierRes.getErrorMsg(), Trace("", ASTNodeType::ForStatement) };

	std::unique_ptr<TypeNode> type;
	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon.getErrorMsg(), Trace("", ASTNodeType::ForStatement) };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { typeRes.getErrorMsg(), Trace("", ASTNodeType::ForStatement) };
		type = typeRes.moveValue();
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->' after variable");
	if(arrow.hasError()) return { arrow.getErrorMsg(), Trace("", ASTNodeType::ForStatement) };

	Result<std::unique_ptr<ASTNode>> iterator = parseExpression();
	if(iterator.hasError()) return iterator;

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after expression");
	if(rightParen.hasError()) return { rightParen.getErrorMsg(), Trace("", ASTNodeType::ForStatement) };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace("", ASTNodeType::ForStatement) };

	return { std::make_unique<ForStatementNode>(
			identifierRes.getValue().value,
			dynamic_unique_cast<ExpressionNode>(iterator.moveValue()),
			dynamic_unique_cast<StatementNode>(body.moveValue()),
			std::move(type)
	) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhileStatement() {
	Result<Token> keywordRes = advance();
	if(keywordRes.hasError()) return {keywordRes.getErrorMsg(), Trace("", ASTNodeType::WhileStatement) };
	Token keyword = keywordRes.getValue();

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if(leftParen.hasError()) return { leftParen.getErrorMsg(), Trace("", ASTNodeType::WhileStatement) };

	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if(condition.hasError()) return { std::move(condition), Trace("", ASTNodeType::WhileStatement) };

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if(rightParen.hasError()) return { rightParen.getErrorMsg(), Trace("", ASTNodeType::WhileStatement) };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace("", ASTNodeType::WhileStatement) };

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token = advance();
		if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::WhileStatement) };

		elseStatement = parseStatement();
		if(elseStatement.hasError()) return { std::move(elseStatement), Trace("", ASTNodeType::WhileStatement) };
	}
	return { std::make_unique<WhileStatementNode>(keyword.value == "dowhile",
												  dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
												  dynamic_unique_cast<StatementNode>(body.moveValue()),
												  dynamic_unique_cast<StatementNode>(elseStatement.moveValue())) };
}


Result<std::unique_ptr<ASTNode>> Parser::parseReturnStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::ReturnStatement) };

	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return { std::move(expr), Trace("", ASTNodeType::ReturnStatement) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg(), Trace("", ASTNodeType::ReturnStatement) };

	return { std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBreakStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::BreakStatement) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg(), Trace("", ASTNodeType::BreakStatement) };

	return { std::make_unique<BreakStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseContinueStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::ContinueStatement) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg(), Trace("", ASTNodeType::ContinueStatement) };

	return { std::make_unique<ContinueStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseTypeAlias() {
	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ASTNode>> Parser::parseClassDeclaration() {
	Result<Token> classKeyword = advance();
	if(classKeyword.hasError()) return { classKeyword.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

	Result<Token> className = advance();
	if(className.hasError()) return { className.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

	std::vector<ConstructorStatementNode> constructors;
	std::vector<ClassDeclarationNode::ClassMember> members;

//  TODO: in the very far future
//	if(peek().type == Token::RightParen) {
//
//	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after class declaration");
	if(leftBrace.hasError()) return { leftBrace.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

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
		if(member.hasError()) return { std::move(member), Trace("", ASTNodeType::ClassDeclaration) };

		members.emplace_back(dynamic_unique_cast<StatementNode>(member.moveValue()), visibility);
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after class declaration");
	if(rightBrace.hasError()) return { rightBrace.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> Parser::parseConstructorArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;
	std::string initializes;

	if(peek().type == Token::LeftParen) {
		Result<Token> leftParen = expect(Token::LeftParen, "Expected '('");
		if(leftParen.hasError()) return { leftParen.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

		Result<Token> initIdentifier = expect(Token::Identifier, "Expected identifier");
		if(initIdentifier.hasError()) return { initIdentifier.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };
		initializes = initIdentifier.getValue().value;

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if(rightParen.hasError()) return { rightParen.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };
	}

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { typeRes.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

		type = typeRes.moveValue();
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if(assign.hasError()) return { assign.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if(valueRes.hasError()) return { valueRes.getErrorMsg(), Trace("", ASTNodeType::ClassDeclaration) };

		value = dynamic_unique_cast<ExpressionNode>(valueRes.moveValue());
	}

	return { std::make_unique<ConstructorStatementNode::ConstructorArgument>(argIdentifier.getValue().value, initializes, std::move(type), std::move(value)) };

}

Result<std::unique_ptr<ASTNode>> Parser::parseConstructorStatement() {
	Result<Token> constructorKeyword = advance();
	if(constructorKeyword.hasError()) return { constructorKeyword.getErrorMsg(), Trace("", ASTNodeType::ConstructorStatement) };

	std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after constructor");
	if(leftParen.hasError()) return { leftParen.getErrorMsg(), Trace("", ASTNodeType::ConstructorStatement) };

	while(true) {
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> arg = parseConstructorArgument();
		if(arg.hasError()) return { arg.getErrorMsg(), Trace("", ASTNodeType::ConstructorStatement) };

		args.push_back(arg.moveValue());

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if(comma.hasError()) return { comma.getErrorMsg(), Trace("", ASTNodeType::ConstructorStatement) };

	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after constructor");
	if(rightParen.hasError()) return { rightParen.getErrorMsg(), Trace("", ASTNodeType::ConstructorStatement) };

	if(peek().type == Token::Semicolon) {
		return { std::make_unique<ConstructorStatementNode>(std::move(args), nullptr) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace("", ASTNodeType::ConstructorStatement) };

	return { std::make_unique<ConstructorStatementNode>(std::move(args), body.moveValue()) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBlock() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::Block) };

	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		Result<std::unique_ptr<ASTNode>> statement = parseStatement();
		if(statement.hasError()) return { std::move(statement), Trace("", ASTNodeType::Block) };

		if(statement.getValue()->getNodeGroup() == ASTNodeGroup::Expression) {
			Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
			if(semiColon.hasError()) {
				semiColon.addToTrace(Trace("", statement.getValue()->getNodeType()));
				return { semiColon, Trace("", ASTNodeType::Block) };
			}
		}

		body.push_back(statement.moveValue());
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after opening '{'");
	if(rightBrace.hasError()) return { rightBrace.getErrorMsg(), Trace("", ASTNodeType::Block) };

	return { std::make_unique<BlockNode>(std::move(body)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseImplStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::ImplStatement) };

	Result<Token> className = expect(Token::Identifier, "Expected Class Name");
	if(className.hasError()) return { className.getErrorMsg(), Trace("", ASTNodeType::ImplStatement) };

	Result<Token> keyword = advance();
	if(keyword.hasError()) return { keyword.getErrorMsg(), Trace("", ASTNodeType::ImplStatement) };

	MemberVisibility visibility = MemberVisibility::PRI;
	if(	keyword.getValue().type == Token::KeywordPub ||
		keyword.getValue().type == Token::KeywordPri ||
		keyword.getValue().type == Token::KeywordPro) {
		visibility = 	keyword.getValue().type == Token::KeywordPub ? MemberVisibility::PUB :
						keyword.getValue().type == Token::KeywordPri ? MemberVisibility::PRI :
						MemberVisibility::PRO;

		keyword = advance();
		if(keyword.hasError()) return { keyword.getErrorMsg(), Trace("", ASTNodeType::ImplStatement) };
	}

	if(keyword.getValue().type != Token::KeywordFunc && keyword.getValue().type != Token::KeywordDef) return { "Expected variable or function declaration", Trace("", ASTNodeType::ImplStatement) };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace("", ASTNodeType::ImplStatement)};

	return { std::make_unique<ImplStatementNode>(className.getValue().value,
											   dynamic_unique_cast<StatementNode>(body.moveValue()), visibility) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseYieldStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token.getErrorMsg(), Trace("", ASTNodeType::YieldStatement) };

	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return { std::move(expr), Trace("", ASTNodeType::YieldStatement) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon.getErrorMsg(), Trace("", ASTNodeType::YieldStatement) };

	return { std::make_unique<YieldStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue())) };
}
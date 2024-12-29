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
	if(keywordError.hasError()) return { keywordError, Trace(ASTNodeType::VariableDeclaration, keywordError.getValue().position) };
	Token keyword = keywordError.getValue();

	Result<Token> identifierError = expect(Token::Identifier, "Expected identifier after declaration");
	if(identifierError.hasError()) return { identifierError, Trace(ASTNodeType::VariableDeclaration, identifierError.getValue().position) };
	Token identifier = identifierError.getValue();

	Result<std::unique_ptr<TypeNode>> type;
	Result<std::unique_ptr<ASTNode>> value;

	if(peek().type == Token::Colon) {
		Result<Token> token = advance();
		if(token.hasError()) return { token, Trace(ASTNodeType::VariableDeclaration, token.getValue().position) };
		
		type = parseType();
		if(type.hasError()) return { std::move(type), Trace(ASTNodeType::VariableDeclaration, keyword.position) };
	}

	if(peek().type == Token::Assign) {
		Result<Token> token = advance();
		if(token.hasError()) return { token, Trace(ASTNodeType::VariableDeclaration, token.getValue().position) };

		value = parseExpression();
		if(value.hasError()) return { std::move(value), Trace(ASTNodeType::VariableDeclaration, keyword.position) };
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::VariableDeclaration, semiColon.getValue().position) };

	if(type.isNull() && value.isNull()) {
//		return { "Expected either value or type, in variable declaration" };
	}

	return { std::make_unique<VariableDeclarationNode>((keyword.value == "const"), !type.isNull() ? type.moveValue() : nullptr,
													   !value.isNull() ? dynamic_unique_cast<ExpressionNode>(value.moveValue()) : nullptr,
													   std::move(identifier.value)) };
}

Result<std::unique_ptr<FunctionArgument>> Parser::parseFunctionArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier, Trace(ASTNodeType::FunctionArgument, argIdentifier.getValue().position) };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon, Trace(ASTNodeType::FunctionArgument, colon.getValue().position) };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { std::move(typeRes), Trace(ASTNodeType::FunctionArgument, argIdentifier.getValue().position) };

		type = typeRes.moveValue();
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if(assign.hasError()) return { assign, Trace(ASTNodeType::FunctionArgument, assign.getValue().position) };
		
		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if(valueRes.hasError()) return { std::move(valueRes), Trace(ASTNodeType::FunctionArgument, argIdentifier.getValue().position) };

		value = dynamic_unique_cast<ExpressionNode>(valueRes.moveValue());
	}

	return { std::make_unique<FunctionArgument>(std::move(type), std::move(value), std::move(argIdentifier.getValue().value)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::FunctionDefinition, token.getValue().position) };

	
	Result<std::unique_ptr<ASTNode>> identifier = parseMemberAccessExpression();
	if(identifier.hasError()) return { std::move(identifier), Trace(ASTNodeType::FunctionDefinition, token.getValue().position) };

	if(!identifier.isNull() && identifier.getValue()->getNodeType() == ASTNodeType::Identifier) {
		auto tempIdentifier = dynamic_cast<IdentifierNode*>(identifier.getValue().get());
		if(tempIdentifier->identifier == "main") mainCounter++;
	}

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after function declaration");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::FunctionDefinition, leftParen.getValue().position) };

	std::vector<std::unique_ptr<FunctionArgument>> args;
	while(peek().type != Token::RightParen) {
		Token::Position argPos = peek().position;
		Result<std::unique_ptr<FunctionArgument>> arg = parseFunctionArgument();
		if(arg.hasError()) return { std::move(arg), Trace(ASTNodeType::FunctionDefinition, argPos) };

		args.push_back(arg.moveValue());

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if(comma.hasError()) return { comma, Trace(ASTNodeType::FunctionDefinition, comma.getValue().position) };
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function declaration");
	if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::FunctionDefinition, rightParen.getValue().position) };

	Result<std::unique_ptr<TypeNode>> returnType;
	if(peek().type != Token::LeftBrace) {
		Token::Position typePos = peek().position;
		returnType = parseType();
		if (returnType.hasError()) return { std::move(returnType), Trace(ASTNodeType::FunctionDefinition, typePos) };
	}

	if(peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if(semicolon.hasError()) return { semicolon, Trace(ASTNodeType::FunctionDefinition, semicolon.getValue().position) };
		return { std::make_unique<FunctionDefinitionNode>(returnType.moveValue(), std::move(args), dynamic_unique_cast<ExpressionNode>(identifier.moveValue())) };
	}

	if(peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if(arrow.hasError()) return { arrow, Trace(ASTNodeType::FunctionDeclaration, arrow.getValue().position) };

		Token::Position exprPos = peek().position;
		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::FunctionDeclaration, exprPos) };

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue()), false);

		Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after inline function");
		if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::FunctionDeclaration, semiColon.getValue().position) };

		return { std::make_unique<FunctionDeclarationNode>(returnType.moveValue(), std::move(args), std::move(body), dynamic_unique_cast<ExpressionNode>(identifier.moveValue())) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::FunctionDeclaration, token.getValue().position) };

	return { std::make_unique<FunctionDeclarationNode>(
			returnType.isNull()? nullptr :returnType.moveValue(),
			std::move(args),
			body.moveValue(),
			dynamic_unique_cast<ExpressionNode>(identifier.moveValue()))
	};
}

Result<std::unique_ptr<ASTNode>> Parser::parseIfStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::IfStatement, token.getValue().position) };

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::IfStatement, leftParen.getValue().position) };

	Token::Position conditionPos = peek().position;
	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if(condition.hasError()) return { std::move(condition), Trace(ASTNodeType::IfStatement, conditionPos) };

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::IfStatement, rightParen.getValue().position) };

	Token::Position bodyPos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::IfStatement, bodyPos) };

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token1 = advance();
		if(token1.hasError()) return { token1, Trace(ASTNodeType::IfStatement, token.getValue().position) };


		elseStatement = parseStatement();
		if(elseStatement.hasError()) return { std::move(elseStatement), Trace(ASTNodeType::IfStatement, token1.getValue().position) };
	}
	return { std::make_unique<IfStatementNode>(dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
	        dynamic_unique_cast<StatementNode>(body.moveValue()),
			dynamic_unique_cast<StatementNode>(elseStatement.moveValue())) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseForStatement() {
	Result<Token> keyword = advance();
	if(keyword.hasError()) return { keyword, Trace(ASTNodeType::ForStatement, keyword.getValue().position) };
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after for statement");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::ForStatement, leftParen.getValue().position) };

	Result<Token> identifierRes = expect(Token::Identifier, "Expected identifier");
	if(identifierRes.hasError()) return { identifierRes, Trace(ASTNodeType::ForStatement, identifierRes.getValue().position) };

	std::unique_ptr<TypeNode> type;
	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon, Trace(ASTNodeType::ForStatement, colon.getValue().position) };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { std::move(typeRes), Trace(ASTNodeType::ForStatement, keyword.getValue().position) };
		type = typeRes.moveValue();
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->' after variable");
	if(arrow.hasError()) return { arrow, Trace(ASTNodeType::ForStatement, arrow.getValue().position) };

	Result<std::unique_ptr<ASTNode>> iterator = parseExpression();
	if(iterator.hasError()) return iterator;

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after expression");
	if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::ForStatement, rightParen.getValue().position) };

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::ForStatement, keyword.getValue().position) };

	return { std::make_unique<ForStatementNode>(
			identifierRes.getValue().value,
			dynamic_unique_cast<ExpressionNode>(iterator.moveValue()),
			dynamic_unique_cast<StatementNode>(body.moveValue()),
			std::move(type)
	) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhileStatement() {
	Result<Token> keywordRes = advance();
	if(keywordRes.hasError()) return {keywordRes, Trace(ASTNodeType::WhileStatement, keywordRes.getValue().position) };
	Token keyword = keywordRes.getValue();

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::WhileStatement, leftParen.getValue().position) };

	Token::Position conditionPos = peek().position;
	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if(condition.hasError()) return { std::move(condition), Trace(ASTNodeType::WhileStatement, conditionPos) };

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::WhileStatement, rightParen.getValue().position) };

	Token::Position bodyPos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::WhileStatement, bodyPos) };

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token = advance();
		if(token.hasError()) return { token, Trace(ASTNodeType::WhileStatement, token.getValue().position) };

		elseStatement = parseStatement();
		if(elseStatement.hasError()) return { std::move(elseStatement), Trace(ASTNodeType::WhileStatement, token.getValue().position) };
	}
	return { std::make_unique<WhileStatementNode>(keyword.value == "dowhile",
												  dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
												  dynamic_unique_cast<StatementNode>(body.moveValue()),
												  dynamic_unique_cast<StatementNode>(elseStatement.moveValue())) };
}


Result<std::unique_ptr<ASTNode>> Parser::parseReturnStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::ReturnStatement, token.getValue().position) };

	bool isSuper = false;
	if(peek().type == Token::KeywordSuper) {
		isSuper = true;
		Result<Token> super = advance();
		if(super.hasError()) return { super, Trace(ASTNodeType::ReturnStatement, super.getValue().position ) };
	}

	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::ReturnStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::ReturnStatement, semiColon.getValue().position) };

	return { std::make_unique<ReturnStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue()), isSuper) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBreakStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::BreakStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::BreakStatement, semiColon.getValue().position) };

	return { std::make_unique<BreakStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseContinueStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::ContinueStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::ContinueStatement, semiColon.getValue().position) };

	return { std::make_unique<ContinueStatementNode>() };
}

Result<std::unique_ptr<ASTNode>> Parser::parseTypeAlias() {
	Result<Token> typeAlias = advance();
	if (typeAlias.hasError())
		return { typeAlias, Trace(ASTNodeType::TypeAlias, typeAlias.getValue().position) };

	Result<Token> identifier = expect(Token::Identifier, "Expected identifier after using");
	if(identifier.hasError()) return { identifier, Trace(ASTNodeType::TypeAlias, identifier.getValue().position) };

	Result<Token> assign = expect(Token::Assign, "Expected '=' after using");
	if(assign.hasError()) return { assign, Trace(ASTNodeType::TypeAlias, assign.getValue().position) };

	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> type = parseType();
	if(type.hasError()) return { std::move(type), Trace(ASTNodeType::TypeAlias, pos) };

	return { std::make_unique<TypeAliasNode>(
			identifier.getValue().value,
			type.moveValue()
		) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseClassDeclaration() {
	Result<Token> classKeyword = advance();
	if(classKeyword.hasError()) return { classKeyword, Trace(ASTNodeType::ClassDeclaration, classKeyword.getValue().position) };

	Result<Token> className = advance();
	if(className.hasError()) return { className, Trace(ASTNodeType::ClassDeclaration, className.getValue().position) };

	std::vector<ConstructorStatementNode> constructors;
	std::vector<ClassDeclarationNode::ClassMember> members;

//  TODO: in the very far future
//	if(peek().type == Token::RightParen) {
//
//	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after class declaration");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::ClassDeclaration, leftBrace.getValue().position) };

	while(peek().type != Token::RightBrace) {
		MemberVisibility visibility = MemberVisibility::PRI;

		if(	peek().type == Token::KeywordPub ||
			peek().type == Token::KeywordPri ||
			peek().type == Token::KeywordPro) {
			Result<Token> visKeyword = advance();
			if(visKeyword.hasError()) return { visKeyword, Trace(ASTNodeType::ClassDeclaration, visKeyword.getValue().position) };

			visibility = 	visKeyword.getValue().type == Token::KeywordPub ? MemberVisibility::PUB :
							visKeyword.getValue().type == Token::KeywordPri ? MemberVisibility::PRI :
							MemberVisibility::PRO;

		}

		if(peek().type != Token::KeywordDef &&
		   peek().type == Token::KeywordFunc &&
		   peek().type != Token::KeywordConstructor)
			return {
			"Expected either function declaration, variable declaration or constructor declaration",
			"",
			Trace(ASTNodeType::ClassDeclaration, peek().position)
		};

		Result<std::unique_ptr<ASTNode>> member = parseStatement();
		if(member.hasError()) return { std::move(member), Trace(ASTNodeType::ClassDeclaration, classKeyword.getValue().position) };

		members.emplace_back(dynamic_unique_cast<StatementNode>(member.moveValue()), visibility);
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after class declaration");
	if(rightBrace.hasError()) return { rightBrace, Trace(ASTNodeType::ClassDeclaration, rightBrace.getValue().position) };

	return std::unique_ptr<ASTNode>();
}

Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> Parser::parseConstructorArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier, Trace(ASTNodeType::ClassDeclaration, argIdentifier.getValue().position) };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;
	std::string initializes;

	if(peek().type == Token::LeftParen) {
		Result<Token> leftParen = expect(Token::LeftParen, "Expected '('");
		if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::ClassDeclaration, leftParen.getValue().position) };

		Result<Token> initIdentifier = expect(Token::Identifier, "Expected identifier");
		if(initIdentifier.hasError()) return { initIdentifier, Trace(ASTNodeType::ClassDeclaration, initIdentifier.getValue().position) };
		initializes = initIdentifier.getValue().value;

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::ClassDeclaration, rightParen.getValue().position) };
	}

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon, Trace(ASTNodeType::ClassDeclaration, colon.getValue().position) };

		
		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { std::move(typeRes), Trace(ASTNodeType::ClassDeclaration, argIdentifier.getValue().position) };

		type = typeRes.moveValue();
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if(assign.hasError()) return { assign, Trace(ASTNodeType::ClassDeclaration, assign.getValue().position) };

		
		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if(valueRes.hasError()) return { std::move(valueRes), Trace(ASTNodeType::ClassDeclaration, argIdentifier.getValue().position) };

		value = dynamic_unique_cast<ExpressionNode>(valueRes.moveValue());
	}

	return { std::make_unique<ConstructorStatementNode::ConstructorArgument>(argIdentifier.getValue().value, initializes, std::move(type), std::move(value)) };

}

Result<std::unique_ptr<ASTNode>> Parser::parseConstructorStatement() {
	Result<Token> constructorKeyword = advance();
	if(constructorKeyword.hasError()) return { constructorKeyword, Trace(ASTNodeType::ConstructorStatement, constructorKeyword.getValue().position) };

	std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after constructor");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::ConstructorStatement, leftParen.getValue().position) };

	while(true) {
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> arg = parseConstructorArgument();
		if(arg.hasError()) return { std::move(arg), Trace(ASTNodeType::ConstructorStatement, constructorKeyword.getValue().position) };

		args.push_back(arg.moveValue());

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if(comma.hasError()) return { comma, Trace(ASTNodeType::ConstructorStatement, comma.getValue().position) };

	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after constructor");
	if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::ConstructorStatement, rightParen.getValue().position) };

	if(peek().type == Token::Semicolon) {
		return { std::make_unique<ConstructorStatementNode>(std::move(args), nullptr) };
	}

	
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::ConstructorStatement, constructorKeyword.getValue().position) };

	return { std::make_unique<ConstructorStatementNode>(std::move(args), body.moveValue()) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBlock() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::Block, token.getValue().position) };

	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		Result<std::unique_ptr<ASTNode>> statement = parseStatement();
		if(statement.hasError()) return { std::move(statement), Trace(ASTNodeType::Block, token.getValue().position) };

		if(statement.getValue()->getNodeGroup() == ASTNodeGroup::Expression) {
			Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
			if(semiColon.hasError()) {
				Token::Position exprPos = peek().position;
				semiColon.addToTrace(Trace(statement.getValue()->getNodeType(), exprPos));
				return { semiColon, Trace(ASTNodeType::Block, semiColon.getValue().position) };
			}
		}

		body.push_back(statement.moveValue());
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after opening '{'");
	if(rightBrace.hasError()) return { rightBrace, Trace(ASTNodeType::Block, rightBrace.getValue().position) };

	return { std::make_unique<BlockNode>(std::move(body)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseImplStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::ImplStatement, token.getValue().position) };

	Result<Token> className = expect(Token::Identifier, "Expected Class Name");
	if(className.hasError()) return { className, Trace(ASTNodeType::ImplStatement, className.getValue().position) };

	Result<Token> keyword = advance();
	if(keyword.hasError()) return { keyword, Trace(ASTNodeType::ImplStatement, keyword.getValue().position) };

	MemberVisibility visibility = MemberVisibility::PRI;
	if(	keyword.getValue().type == Token::KeywordPub ||
		keyword.getValue().type == Token::KeywordPri ||
		keyword.getValue().type == Token::KeywordPro) {
		visibility = 	keyword.getValue().type == Token::KeywordPub ? MemberVisibility::PUB :
						keyword.getValue().type == Token::KeywordPri ? MemberVisibility::PRI :
						MemberVisibility::PRO;

		keyword = advance();
		if(keyword.hasError()) return { keyword, Trace(ASTNodeType::ImplStatement, keyword.getValue().position) };
	}

	
	if(keyword.getValue().type != Token::KeywordFunc && keyword.getValue().type != Token::KeywordDef)
		return { "Expected variable or function declaration", "To be implemented", Trace(ASTNodeType::ImplStatement, token.getValue().position) };

	Token::Position bodyPos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::ImplStatement, bodyPos)};

	return { std::make_unique<ImplStatementNode>(className.getValue().value,
											   dynamic_unique_cast<StatementNode>(body.moveValue()), visibility) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseYieldStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::YieldStatement, token.getValue().position) };

	
	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::YieldStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::YieldStatement, semiColon.getValue().position) };

	return { std::make_unique<YieldStatementNode>(dynamic_unique_cast<ExpressionNode>(expr.moveValue())) };
}
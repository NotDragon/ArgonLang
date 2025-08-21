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
		case Token::KeywordEnum:
			return { parseEnumDeclaration() };
		case Token::KeywordConstraint:
			return { parseConstraintDeclaration() };
		case Token::KeywordModule:
			return { parseModuleDeclaration() };
		case Token::KeywordImport:
			return { parseImportStatement() };
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

	// Parse the left-hand side (can be simple name, single pattern, or compound patterns)
	bool isSimple = false;
	bool isSinglePattern = false;
	bool isCompoundPattern = false;
	std::string name = "";
	std::unique_ptr<PatternNode> pattern = nullptr;
	std::vector<std::unique_ptr<PatternNode>> compoundPatterns;
	
	// Parse first element
	if(peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
		// Starts with a pattern
		Result<std::unique_ptr<PatternNode>> firstPattern = parsePattern();
		if(firstPattern.hasError()) return { std::move(firstPattern), Trace(ASTNodeType::VariableDeclaration, keyword.position) };
		
		// Check if there's a comma (compound pattern)
		if(peek().type == Token::Comma) {
			// This is compound destructuring: [first], rest = ...
			isCompoundPattern = true;
			compoundPatterns.push_back(firstPattern.moveValue());
			
			// Parse remaining patterns
			while(peek().type == Token::Comma) {
				advance(); // consume comma
				
				if(peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
					// Another pattern
					Result<std::unique_ptr<PatternNode>> nextPattern = parsePattern();
					if(nextPattern.hasError()) return { std::move(nextPattern), Trace(ASTNodeType::VariableDeclaration, keyword.position) };
					compoundPatterns.push_back(nextPattern.moveValue());
				} else if(peek().type == Token::Identifier) {
					// Rest identifier
					Result<Token> restId = advance();
					if(restId.hasError()) return { restId, Trace(ASTNodeType::VariableDeclaration, restId.getValue().position) };
					compoundPatterns.push_back(std::make_unique<IdentifierPatternNode>(restId.getValue().position, restId.getValue().value));
				} else {
					return { "Expected pattern or identifier after comma in compound destructuring", "", Trace(ASTNodeType::VariableDeclaration, peek().position) };
				}
			}
		} else {
			// Single pattern destructuring: def [a, b] = ...
			isSinglePattern = true;
			pattern = firstPattern.moveValue();
		}
	} else if(peek().type == Token::Identifier) {
		// Could be simple declaration or compound starting with identifier
		Result<Token> firstId = advance();
		if(firstId.hasError()) return { firstId, Trace(ASTNodeType::VariableDeclaration, firstId.getValue().position) };
		
		if(peek().type == Token::Comma) {
			// Compound destructuring starting with identifier: def rest, [last] = ...
			isCompoundPattern = true;
			compoundPatterns.push_back(std::make_unique<IdentifierPatternNode>(firstId.getValue().position, firstId.getValue().value));
			
			// Parse remaining patterns
			while(peek().type == Token::Comma) {
				advance(); // consume comma
				
				if(peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
					Result<std::unique_ptr<PatternNode>> nextPattern = parsePattern();
					if(nextPattern.hasError()) return { std::move(nextPattern), Trace(ASTNodeType::VariableDeclaration, keyword.position) };
					compoundPatterns.push_back(nextPattern.moveValue());
				} else if(peek().type == Token::Identifier) {
					Result<Token> restId = advance();
					if(restId.hasError()) return { restId, Trace(ASTNodeType::VariableDeclaration, restId.getValue().position) };
					compoundPatterns.push_back(std::make_unique<IdentifierPatternNode>(restId.getValue().position, restId.getValue().value));
				} else {
					return { "Expected pattern or identifier after comma in compound destructuring", "", Trace(ASTNodeType::VariableDeclaration, peek().position) };
				}
			}
		} else {
			// Simple declaration: def name = ...
			isSimple = true;
			name = firstId.getValue().value;
		}
	} else {
		return { "Expected identifier or pattern after 'def'", "", Trace(ASTNodeType::VariableDeclaration, keyword.position) };
	}

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
	Token::Position smiColonPos = peek().position;
	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::VariableDeclaration, smiColonPos) };

	if(type.isNull() && value.isNull()) {
//		return { "Expected either value or type, in variable declaration" };
	}

	if(isCompoundPattern) {
		return { std::make_unique<VariableDeclarationNode>(keyword.position, (keyword.value == "const"), 
														   !type.isNull() ? type.moveValue() : nullptr,
														   !value.isNull() ? dynamic_unique_cast<ExpressionNode>(value.moveValue()) : nullptr,
														   std::move(compoundPatterns)) };
	} else if(isSinglePattern) {
		return { std::make_unique<VariableDeclarationNode>(keyword.position, (keyword.value == "const"), 
														   !type.isNull() ? type.moveValue() : nullptr,
														   !value.isNull() ? dynamic_unique_cast<ExpressionNode>(value.moveValue()) : nullptr,
														   std::move(pattern)) };
	} else {
		return { std::make_unique<VariableDeclarationNode>(keyword.position, (keyword.value == "const"), 
														   !type.isNull() ? type.moveValue() : nullptr,
														   !value.isNull() ? dynamic_unique_cast<ExpressionNode>(value.moveValue()) : nullptr,
														   name) };
	}
}

Result<std::unique_ptr<FunctionArgument>> Parser::parseFunctionArgument() {
	// Check for variadic argument: ...args: type
	bool isVariadic = false;
	Token::Position startPos = peek().position;
	
	if(peek().type == Token::Ellipsis) {
		isVariadic = true;
		Result<Token> ellipsis = advance();
		if(ellipsis.hasError()) return { ellipsis.getErrorMsg(), ellipsis.getErrorNote(), Trace(ASTNodeType::FunctionArgument, startPos) };
	}

	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if(argIdentifier.hasError()) return { argIdentifier.getErrorMsg(), argIdentifier.getErrorNote(), Trace(ASTNodeType::FunctionArgument, startPos) };

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon.getErrorMsg(), colon.getErrorNote(), Trace(ASTNodeType::FunctionArgument, startPos) };

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if(typeRes.hasError()) return { typeRes.getErrorMsg(), typeRes.getErrorNote(), Trace(ASTNodeType::FunctionArgument, startPos) };

		// If this is a variadic argument, wrap the type in a VariadicTypeNode
		if(isVariadic) {
			type = std::make_unique<VariadicTypeNode>(startPos, typeRes.moveValue());
		} else {
			type = typeRes.moveValue();
		}
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if(assign.hasError()) return { assign.getErrorMsg(), assign.getErrorNote(), Trace(ASTNodeType::FunctionArgument, startPos) };
		
		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if(valueRes.hasError()) return { valueRes.getErrorMsg(), valueRes.getErrorNote(), Trace(ASTNodeType::FunctionArgument, startPos) };

		value = dynamic_unique_cast<ExpressionNode>(valueRes.moveValue());
	}

	return { std::make_unique<FunctionArgument>(startPos, std::move(type), std::move(value), std::move(argIdentifier.getValue().value)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::FunctionDefinition, token.getValue().position) };

	// Parse generic parameters if present: func<T, U>
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if(peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if(less.hasError()) return { less, Trace(ASTNodeType::FunctionDefinition, lessPos) };

		while(peek().type != Token::Greater) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parseGenericParameter();
			if(param.hasError()) return { std::move(param), Trace(ASTNodeType::FunctionDefinition, pos) };

			genericParams.push_back(param.moveValue());

			if(peek().type == Token::Greater) break;
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between generic parameters");
			if(comma.hasError()) return { comma, Trace(ASTNodeType::FunctionDefinition, commaPos) };
		}

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if(greater.hasError()) return { greater, Trace(ASTNodeType::FunctionDefinition, greaterPos) };
	}

	Result<std::unique_ptr<ASTNode>> identifier = parseMemberAccessExpression();
	if(identifier.hasError()) return { std::move(identifier), Trace(ASTNodeType::FunctionDefinition, token.getValue().position) };

	if(!identifier.isNull() && identifier.getValue()->getNodeType() == ASTNodeType::Identifier) {
		auto tempIdentifier = dynamic_cast<IdentifierNode*>(identifier.getValue().get());
		if(tempIdentifier->identifier == "main") mainCounter++;
	}

	Token::Position leftParenPos = peek().position;
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after function declaration");
	if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::FunctionDefinition, leftParenPos) };

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
	
	// Handle function syntax patterns:
	// 1. func name(args) ReturnType -> expression;  (inline expression with return type)
	// 2. func name(args) -> expression;             (inline expression without return type)
	// 3. func name(args) ReturnType { body }        (function with return type and body)
	// 4. func name(args) { body }                   (function without return type)
	// 5. func name(args);                           (declaration only)
	
	// First, check if we have a return type (not immediately -> or { or ;)
	if(peek().type != Token::Arrow && peek().type != Token::LeftBrace && peek().type != Token::Semicolon) {
		// Parse return type: func name(args) ReturnType ...
		Token::Position typePos = peek().position;
		
		Result<std::unique_ptr<TypeNode>> typeResult = parseType();
		if (!typeResult.hasError()) {
			returnType = std::move(typeResult);
		}
		// If type parsing failed, we'll continue and let the arrow/body/semicolon parsing handle it
	}
	
	// Now check for arrow (inline expression)
	if(peek().type == Token::Arrow) {
		// Arrow syntax for inline expression: -> expression;
		Result<Token> arrow = advance();
		if(arrow.hasError()) return { arrow, Trace(ASTNodeType::FunctionDefinition, arrow.getValue().position) };
		
		Token::Position exprPos = peek().position;
		
		// Parse the inline expression
		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::FunctionDeclaration, exprPos) };

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(exprPos, dynamic_unique_cast<ExpressionNode>(expr.moveValue()), false);

		Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after inline function");
		if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::FunctionDeclaration, semiColon.getValue().position) };

		return { std::make_unique<FunctionDeclarationNode>(token.getValue().position, returnType.moveValue(), std::move(args), std::move(body), dynamic_unique_cast<ExpressionNode>(identifier.moveValue()), std::move(genericParams)) };
	}

	if(peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if(semicolon.hasError()) return { semicolon, Trace(ASTNodeType::FunctionDefinition, semicolon.getValue().position) };
		return { std::make_unique<FunctionDefinitionNode>(token.getValue().position, returnType.moveValue(), std::move(args), dynamic_unique_cast<ExpressionNode>(identifier.moveValue()), std::move(genericParams)) };
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::FunctionDeclaration, token.getValue().position) };

	return { std::make_unique<FunctionDeclarationNode>(
			token.getValue().position,
			returnType.isNull()? nullptr :returnType.moveValue(),
			std::move(args),
			body.moveValue(),
			dynamic_unique_cast<ExpressionNode>(identifier.moveValue()),
			std::move(genericParams))
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
	return { std::make_unique<IfStatementNode>(
			token.getValue().position,
			dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
	        dynamic_unique_cast<StatementNode>(body.moveValue()),
			!elseStatement.isNull()? dynamic_unique_cast<StatementNode>(elseStatement.moveValue()): nullptr) };
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
			keyword.getValue().position,
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
	return { std::make_unique<WhileStatementNode>(
			keyword.position,
			keyword.value == "dowhile",
			dynamic_unique_cast<ExpressionNode>(condition.moveValue()),
			dynamic_unique_cast<StatementNode>(body.moveValue()),
			!elseStatement.isNull()? dynamic_unique_cast<StatementNode>(elseStatement.moveValue()): nullptr) };
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

	return { std::make_unique<ReturnStatementNode>(token.getValue().position, dynamic_unique_cast<ExpressionNode>(expr.moveValue()), isSuper) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseBreakStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::BreakStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::BreakStatement, semiColon.getValue().position) };

	return { std::make_unique<BreakStatementNode>(token.getValue().position) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseContinueStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::ContinueStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::ContinueStatement, semiColon.getValue().position) };

	return { std::make_unique<ContinueStatementNode>(token.getValue().position) };
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

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after type alias");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::TypeAlias, semiColon.getValue().position) };

	return { std::make_unique<TypeAliasNode>(
			typeAlias.getValue().position,
			identifier.getValue().value,
			type.moveValue()
		) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseClassDeclaration() {
	Result<Token> classKeyword = advance();
	if(classKeyword.hasError()) return { classKeyword, Trace(ASTNodeType::ClassDeclaration, classKeyword.getValue().position) };

	Result<Token> className = advance();
	if(className.hasError()) return { className, Trace(ASTNodeType::ClassDeclaration, className.getValue().position) };

	// Parse generic parameters if present: class Name<T, U> { ... }
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if(peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if(less.hasError()) return { less, Trace(ASTNodeType::ClassDeclaration, lessPos) };

		// Parse generic parameters
		do {
			Token::Position paramPos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parseGenericParameter();
			if(param.hasError()) return { std::move(param), Trace(ASTNodeType::ClassDeclaration, paramPos) };
			
			genericParams.push_back(param.moveValue());

			if(peek().type == Token::Comma) {
				Token::Position commaPos = peek().position;
				Result<Token> comma = advance();
				if(comma.hasError()) return { comma, Trace(ASTNodeType::ClassDeclaration, commaPos) };
			} else {
				break;
			}
		} while(true);

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if(greater.hasError()) return { greater, Trace(ASTNodeType::ClassDeclaration, greaterPos) };
	}

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
		   peek().type != Token::KeywordFunc &&
		   peek().type != Token::KeywordConstructor)
			return {
			"Expected either function declaration, variable declaration or constructor declaration",
			"",
			Trace(ASTNodeType::ClassDeclaration, peek().position)
		};

		Result<std::unique_ptr<ASTNode>> member = parseStatement();
		if(member.hasError()) return { std::move(member), Trace(ASTNodeType::ClassDeclaration, classKeyword.getValue().position) };
		Token::Position memberPosition = member.getValue()->position;
		members.emplace_back(memberPosition, dynamic_unique_cast<StatementNode>(member.moveValue()), visibility);
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after class declaration");
	if(rightBrace.hasError()) return { rightBrace, Trace(ASTNodeType::ClassDeclaration, rightBrace.getValue().position) };

	return { std::make_unique<ClassDeclarationNode>(
		classKeyword.getValue().position,
		className.getValue().value,
		std::move(members),
		std::move(genericParams)
	) };
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

	return { std::make_unique<ConstructorStatementNode::ConstructorArgument>(argIdentifier.getValue().position, argIdentifier.getValue().value, initializes, std::move(type), std::move(value)) };

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
		return { std::make_unique<ConstructorStatementNode>(constructorKeyword.getValue().position, std::move(args), nullptr) };
	}

	
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if(body.hasError()) return { std::move(body), Trace(ASTNodeType::ConstructorStatement, constructorKeyword.getValue().position) };

	return { std::make_unique<ConstructorStatementNode>(constructorKeyword.getValue().position, std::move(args), body.moveValue()) };
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

	return { std::make_unique<BlockNode>(token.getValue().position, std::move(body)) };
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

	return { std::make_unique<ImplStatementNode>(
			token.getValue().position,
			className.getValue().value,
			dynamic_unique_cast<StatementNode>(body.moveValue()), visibility) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseYieldStatement() {
	Result<Token> token = advance();
	if(token.hasError()) return { token, Trace(ASTNodeType::YieldStatement, token.getValue().position) };

	
	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if(expr.hasError()) return { std::move(expr), Trace(ASTNodeType::YieldStatement, token.getValue().position) };

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if(semiColon.hasError()) return { semiColon, Trace(ASTNodeType::YieldStatement, semiColon.getValue().position) };

	return { std::make_unique<YieldStatementNode>(
					token.getValue().position,
					dynamic_unique_cast<ExpressionNode>(expr.moveValue())
			        ) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseEnumDeclaration() {
	Result<Token> enumKeyword = advance();
	if(enumKeyword.hasError()) return { enumKeyword, Trace(ASTNodeType::EnumDeclaration, enumKeyword.getValue().position) };

	bool isUnion = false;
	if(peek().type == Token::KeywordUnion) {
		isUnion = true;
		Result<Token> unionKeyword = advance();
		if(unionKeyword.hasError()) return { unionKeyword, Trace(ASTNodeType::EnumDeclaration, unionKeyword.getValue().position) };
	}

	Result<Token> enumName = expect(Token::Identifier, "Expected enum name");
	if(enumName.hasError()) return { enumName, Trace(ASTNodeType::EnumDeclaration, enumName.getValue().position) };

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after enum name");
	if(leftBrace.hasError()) return { leftBrace, Trace(ASTNodeType::EnumDeclaration, leftBrace.getValue().position) };

	std::vector<EnumDeclarationNode::EnumVariant> variants;
	while(peek().type != Token::RightBrace) {
		Result<Token> variantName = expect(Token::Identifier, "Expected variant name");
		if(variantName.hasError()) return { variantName, Trace(ASTNodeType::EnumDeclaration, variantName.getValue().position) };

		std::vector<std::unique_ptr<TypeNode>> fields;
		if(peek().type == Token::LeftParen) {
			Result<Token> leftParen = advance();
			if(leftParen.hasError()) return { leftParen, Trace(ASTNodeType::EnumDeclaration, leftParen.getValue().position) };

			while(peek().type != Token::RightParen) {
				Token::Position pos = peek().position;
				Result<std::unique_ptr<TypeNode>> fieldType = parseType();
				if(fieldType.hasError()) return { std::move(fieldType), Trace(ASTNodeType::EnumDeclaration, pos) };

				fields.push_back(fieldType.moveValue());

				if(peek().type == Token::RightParen) break;
				Result<Token> comma = expect(Token::Comma, "Expected ',' between enum fields");
				if(comma.hasError()) return { comma, Trace(ASTNodeType::EnumDeclaration, comma.getValue().position) };
			}

			Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after enum fields");
			if(rightParen.hasError()) return { rightParen, Trace(ASTNodeType::EnumDeclaration, rightParen.getValue().position) };
		}

		variants.emplace_back(variantName.getValue().position, variantName.getValue().value, std::move(fields));

		if(peek().type == Token::RightBrace) break;
		Result<Token> comma = expect(Token::Comma, "Expected ',' between enum variants");
		if(comma.hasError()) return { comma, Trace(ASTNodeType::EnumDeclaration, comma.getValue().position) };
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after enum declaration");
	if(rightBrace.hasError()) return { rightBrace, Trace(ASTNodeType::EnumDeclaration, rightBrace.getValue().position) };

	return { std::make_unique<EnumDeclarationNode>(enumKeyword.getValue().position, enumName.getValue().value, std::move(variants), isUnion) };
}



Result<std::unique_ptr<ASTNode>> Parser::parseConstraintDeclaration() {
	Token::Position startPos = peek().position;
	Result<Token> constraintKeyword = advance();
	if(constraintKeyword.hasError()) return { constraintKeyword, Trace(ASTNodeType::ConstraintDeclaration, startPos) };

	Token::Position namePos = peek().position;
	Result<Token> constraintName = expect(Token::Identifier, "Expected constraint name");
	if(constraintName.hasError()) return { constraintName, Trace(ASTNodeType::ConstraintDeclaration, namePos) };

	// Parse generic parameters if present
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if(peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if(less.hasError()) return { less, Trace(ASTNodeType::ConstraintDeclaration, lessPos) };

		while(peek().type != Token::Greater) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parseGenericParameter();
			if(param.hasError()) return { std::move(param), Trace(ASTNodeType::ConstraintDeclaration, pos) };

			genericParams.push_back(param.moveValue());

			if(peek().type == Token::Greater) break;
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between generic parameters");
			if(comma.hasError()) return { comma, Trace(ASTNodeType::ConstraintDeclaration, commaPos) };
		}

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if(greater.hasError()) return { greater, Trace(ASTNodeType::ConstraintDeclaration, greaterPos) };
	}

	Token::Position assignPos = peek().position;
	Result<Token> assign = expect(Token::Assign, "Expected '=' after constraint declaration");
	if(assign.hasError()) return { assign, Trace(ASTNodeType::ConstraintDeclaration, assignPos) };

	Token::Position exprPos = peek().position;
	Result<std::unique_ptr<ASTNode>> constraintExpressionRes = parseExpression();
	if(constraintExpressionRes.hasError()) return { std::move(constraintExpressionRes), Trace(ASTNodeType::ConstraintDeclaration, exprPos) };
	
	std::unique_ptr<ExpressionNode> constraintExpression = dynamic_unique_cast<ExpressionNode>(constraintExpressionRes.moveValue());

	Token::Position semicolonPos = peek().position;
	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after constraint expression");
	if(semicolon.hasError()) return { semicolon, Trace(ASTNodeType::ConstraintDeclaration, semicolonPos) };

	return { std::make_unique<ConstraintDeclarationNode>(constraintKeyword.getValue().position, constraintName.getValue().value, std::move(genericParams), std::move(constraintExpression)) };
}

Result<std::unique_ptr<GenericParameter>> Parser::parseGenericParameter() {
	Token::Position namePos = peek().position;
	Result<Token> name = expect(Token::Identifier, "Expected generic parameter name");
	if(name.hasError()) return { name, Trace(ASTNodeType::GenericParameter, namePos) };

	std::unique_ptr<TypeNode> constraint = nullptr;
	
	// Check for constraint: T: Number
	if(peek().type == Token::Colon) {
		Token::Position colonPos = peek().position;
		Result<Token> colon = advance();
		if(colon.hasError()) return { colon, Trace(ASTNodeType::GenericParameter, colonPos) };

		Token::Position constraintPos = peek().position;
		Result<std::unique_ptr<TypeNode>> constraintResult = parseType();
		if(constraintResult.hasError()) return { std::move(constraintResult), Trace(ASTNodeType::GenericParameter, constraintPos) };

		constraint = constraintResult.moveValue();
	}

	return { std::make_unique<GenericParameter>(namePos, name.getValue().value, std::move(constraint)) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseModuleDeclaration() {
	Result<Token> moduleKeyword = advance();
	if(moduleKeyword.hasError()) return { moduleKeyword, Trace(ASTNodeType::ModuleDeclaration, moduleKeyword.getValue().position) };

	Result<Token> moduleName = expect(Token::Identifier, "Expected module name");
	if(moduleName.hasError()) return { moduleName, Trace(ASTNodeType::ModuleDeclaration, moduleName.getValue().position) };

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after module declaration");
	if(semicolon.hasError()) return { semicolon, Trace(ASTNodeType::ModuleDeclaration, semicolon.getValue().position) };

	// For now, we'll create a simple module declaration without body
	// In a full implementation, we'd parse the module body
	return { std::make_unique<ModuleDeclarationNode>(moduleKeyword.getValue().position, moduleName.getValue().value, std::vector<std::unique_ptr<StatementNode>>()) };
}

Result<std::unique_ptr<ASTNode>> Parser::parseImportStatement() {
	Result<Token> importKeyword = advance();
	if(importKeyword.hasError()) return { importKeyword, Trace(ASTNodeType::ImportStatement, importKeyword.getValue().position) };

	Result<Token> moduleName = expect(Token::Identifier, "Expected module name");
	if(moduleName.hasError()) return { moduleName, Trace(ASTNodeType::ImportStatement, moduleName.getValue().position) };

	std::vector<std::string> importedItems;
	if(peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if(arrow.hasError()) return { arrow, Trace(ASTNodeType::ImportStatement, arrow.getValue().position) };

		// Parse imported items
		do {
			Result<Token> item = expect(Token::Identifier, "Expected imported item name");
			if(item.hasError()) return { item, Trace(ASTNodeType::ImportStatement, item.getValue().position) };

			importedItems.push_back(item.getValue().value);

			if(peek().type == Token::Comma) {
				Result<Token> comma = advance();
				if(comma.hasError()) return { comma, Trace(ASTNodeType::ImportStatement, comma.getValue().position) };
			} else {
				break;
			}
		} while(true);
	}

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after import statement");
	if(semicolon.hasError()) return { semicolon, Trace(ASTNodeType::ImportStatement, semicolon.getValue().position) };

	return { std::make_unique<ImportStatementNode>(importKeyword.getValue().position, moduleName.getValue().value, std::move(importedItems)) };
}
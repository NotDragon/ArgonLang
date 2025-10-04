#include "backend/Parser.h"
#include "Error/Error.h"
#include "Error/Result.h"
#include <stdexcept>

using namespace ArgonLang;

Result<std::unique_ptr<ASTNode>> Parser::parseStatement() {
	switch (peek().type) {
		case Token::KeywordUnion:
			return parseUnionDeclaration();
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
		case Token::KeywordConstructor:
			return parseConstructorStatement();
		case Token::KeywordImpl:
			return parseImplStatement();
		case Token::KeywordUsing:
			return parseTypeAlias();
		case Token::KeywordYield:
			return parseYieldStatement();
		case Token::KeywordEnum:
			return parseEnumDeclaration();
		case Token::KeywordConstraint:
			return parseConstraintDeclaration();
		case Token::KeywordModule:
			return parseModuleDeclaration();
		case Token::KeywordImport:
			return parseImportStatement();
		case Token::KeywordPar:
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

Result<std::unique_ptr<ASTNode>> Parser::parseVariableDeclaration() {
	Result<Token> keywordResult = advance();
	if (!keywordResult.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keywordResult.error());
	}
	Token keyword = keywordResult.value();

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
		if (!firstPattern.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(firstPattern.error());
		}
		
		// Check if there's a comma (compound pattern)
		if(peek().type == Token::Comma) {
			// This is compound destructuring: [first], rest = ...
			isCompoundPattern = true;
			compoundPatterns.push_back(std::move(firstPattern.value()));
			
			// Parse remaining patterns
			while(peek().type == Token::Comma) {
				advance(); // consume comma
				
				if(peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
					// Another pattern
					Result<std::unique_ptr<PatternNode>> nextPattern = parsePattern();
					if (!nextPattern.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(nextPattern.error());
					}
					compoundPatterns.push_back(std::move(nextPattern.value()));
				} else if(peek().type == Token::Identifier) {
					// Rest identifier
					Result<Token> restId = advance();
					if (!restId.has_value()) {
						return Err<std::unique_ptr<ASTNode>>(restId.error());
					}
					compoundPatterns.push_back(std::make_unique<IdentifierPatternNode>(restId.value().position, restId.value().value));
				} else {
					Token currentToken = peek();
				Error error = create_parse_error(ErrorType::UnexpectedToken,
					"Expected pattern or identifier after comma in compound destructuring",
					currentToken.position);
				error.withExpected("pattern or identifier").withActual(currentToken.value);
				return Err<std::unique_ptr<ASTNode>>(error);
				}
			}
		} else {
			// Single pattern destructuring: def [a, b] = ...
			isSinglePattern = true;
			pattern = std::move(firstPattern.value());
		}
	} else if(peek().type == Token::Identifier) {
		// Could be simple declaration or compound starting with identifier
		Result<Token> firstId = advance();
		if (!firstId.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(firstId.error());
		}
		
		if(peek().type == Token::Comma) {
			// Compound destructuring starting with identifier: def rest, [last] = ...
			isCompoundPattern = true;
			compoundPatterns.push_back(std::make_unique<IdentifierPatternNode>(firstId.value().position, firstId.value().value));
			
			// Parse remaining patterns
			while(peek().type == Token::Comma) {
				advance(); // consume comma
				
				if(peek().type == Token::LeftBracket || peek().type == Token::LeftBrace) {
				Result<std::unique_ptr<PatternNode>> nextPattern = parsePattern();
				if (!nextPattern.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(nextPattern.error());
				}
				compoundPatterns.push_back(std::move(nextPattern.value()));
				} else if(peek().type == Token::Identifier) {
				Result<Token> restId = advance();
				if (!restId.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(restId.error());
				}
					compoundPatterns.push_back(std::make_unique<IdentifierPatternNode>(restId.value().position, restId.value().value));
				} else {
					return Err<std::unique_ptr<ASTNode>>(create_parse_error(ErrorType::UnexpectedToken, "Expected pattern or identifier after comma in compound destructuring", peek().position));
				}
			}
		} else {
			// Simple declaration: def name = ...
			isSimple = true;
			name = firstId.value().value;
		}
	} else {
		return Err<std::unique_ptr<ASTNode>>(create_parse_error(ErrorType::UnexpectedToken, "Expected identifier or pattern after 'def'", keyword.position));
	}

	Result<std::unique_ptr<TypeNode>> type;
	Result<std::unique_ptr<ASTNode>> value;

	if(peek().type == Token::Colon) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token.error());
		}
		
		type = parseType();
		if (!type.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(type.error());
		}
	}

	if(peek().type == Token::Assign) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token.error());
		}

		value = parseExpression();
		if (!value.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(value.error());
	}
	}
	Token::Position smiColonPos = peek().position;
	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if (!semiColon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semiColon.error());
	}

	if (!type.has_value() && !value.has_value()) {
//		return { "Expected either value or type, in variable declaration" };
	}

	if(isCompoundPattern) {
		return Ok(std::make_unique<VariableDeclarationNode>(keyword.position, (keyword.value == "const"), 
									   type.has_value() ? std::move(type.value()) : nullptr,
									   value.has_value() ? dynamic_unique_cast<ExpressionNode>(std::move(value.value())) : nullptr,
									   std::move(compoundPatterns)));
	} else if(isSinglePattern) {
		return Ok(std::make_unique<VariableDeclarationNode>(keyword.position, (keyword.value == "const"), 
									   type.has_value() ? std::move(type.value()) : nullptr,
									   value.has_value() ? dynamic_unique_cast<ExpressionNode>(std::move(value.value())) : nullptr,
									   std::move(pattern)));
	} else {
		return Ok(std::make_unique<VariableDeclarationNode>(keyword.position, (keyword.value == "const"), 
									   type.has_value() ? std::move(type.value()) : nullptr,
								   value.has_value() ? dynamic_unique_cast<ExpressionNode>(std::move(value.value())) : nullptr,
									   name));
	}
}

Result<std::unique_ptr<FunctionArgument>> Parser::parseFunctionArgument() {
	Token::Position startPos = peek().position;

	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if (!argIdentifier.has_value()) {
		return Err<std::unique_ptr<FunctionArgument>>(argIdentifier.error());
	}

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(colon.error());
		}

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if (!typeRes.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(typeRes.error());
		}

		type = std::move(typeRes.value());
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (!assign.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(assign.error());
		}
		
		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if (!valueRes.has_value()) {
			return Err<std::unique_ptr<FunctionArgument>>(valueRes.error());
		}

		value = dynamic_unique_cast<ExpressionNode>(std::move(valueRes.value()));
	}

	return Ok(std::make_unique<FunctionArgument>(startPos, std::move(type), std::move(value), argIdentifier.value().value));
}

Result<std::unique_ptr<ASTNode>> Parser::parseFunctionDeclaration() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	// Parse generic parameters if present: func<T, U>
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if(peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if (!less.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(less.error());
		}

		while(peek().type != Token::Greater) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parseGenericParameter();
			if (!param.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(param.error());
			}

			genericParams.push_back(std::move(param.value()));

			if(peek().type == Token::Greater) break;
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between generic parameters");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if (!greater.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(greater.error());
		}
	}
	
	Result<std::unique_ptr<ASTNode>> identifier = parseMemberAccessExpression();
	if (!identifier.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(identifier.error());
	}

	if (identifier.has_value() && identifier.value()->getNodeType() == ASTNodeType::Identifier) {
		auto tempIdentifier = dynamic_cast<IdentifierNode*>(identifier.value().get());
		if(tempIdentifier->identifier == "main") mainCounter++;
	}

	Token::Position leftParenPos = peek().position;
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after function declaration");
	if (!leftParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftParen.error());
	}

	std::vector<std::unique_ptr<FunctionArgument>> args;
	while(peek().type != Token::RightParen) {
		Token::Position argPos = peek().position;
		Result<std::unique_ptr<FunctionArgument>> arg = parseFunctionArgument();
		if (!arg.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arg.error());
		}

		args.push_back(std::move(arg.value()));

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after function declaration");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

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
		if (typeResult.has_value()) {
			returnType = std::move(typeResult);
		}
		// If type parsing failed, we'll continue and let the arrow/body/semicolon parsing handle it
	}
	
	// Now check for arrow (inline expression)
	if(peek().type == Token::Arrow) {
		// Arrow syntax for inline expression: -> expression;
		Result<Token> arrow = advance();
		if (!arrow.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arrow.error());
		}

		Token::Position exprPos = peek().position;
		
		// Parse the inline expression
		Result<std::unique_ptr<ASTNode>> expr = parseExpression();
		if (!expr.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(expr.error());
		}

		std::unique_ptr<ReturnStatementNode> body = std::make_unique<ReturnStatementNode>(exprPos, dynamic_unique_cast<ExpressionNode>(std::move(expr.value())), false);

		Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after inline function");
		if (!semiColon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(semiColon.error());
		}

		return Ok(std::make_unique<FunctionDeclarationNode>(token.value().position, std::move(returnType.value()), std::move(args), std::move(body), dynamic_unique_cast<ExpressionNode>(std::move(identifier.value())), std::move(genericParams)));
	}

	if(peek().type == Token::Semicolon) {
		Result<Token> semicolon = advance();
		if (!semicolon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(semicolon.error());
		}
		return Ok(std::make_unique<FunctionDefinitionNode>(token.value().position, std::move(returnType.value()), std::move(args), dynamic_unique_cast<ExpressionNode>(std::move(identifier.value())), std::move(genericParams)));
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<FunctionDeclarationNode>(
			token.value().position,
			returnType.has_value() ? std::move(returnType.value()) : nullptr,
			std::move(args),
			std::move(body.value()),
			dynamic_unique_cast<ExpressionNode>(std::move(identifier.value())),
			std::move(genericParams)));
}

Result<std::unique_ptr<ASTNode>> Parser::parseIfStatement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after if statement");
	if (!leftParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftParen.error());
	}

	Token::Position conditionPos = peek().position;
	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if (!condition.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(condition.error());
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	Token::Position bodyPos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token1 = advance();
		if (!token1.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token1.error());
		}


		elseStatement = parseStatement();
		if (!elseStatement.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(elseStatement.error());
		}
	}
	return Ok(std::make_unique<IfStatementNode>(
			token.value().position,
			dynamic_unique_cast<ExpressionNode>(std::move(condition.value())),
	        dynamic_unique_cast<StatementNode>(std::move(body.value())),
			elseStatement.has_value() ? dynamic_unique_cast<StatementNode>(std::move(elseStatement.value())) : nullptr));
}

Result<std::unique_ptr<ASTNode>> Parser::parseForStatement() {
	Result<Token> keyword = advance();
	if (!keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keyword.error());
	}
	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after for statement");
	if (!leftParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftParen.error());
	}

	Result<Token> identifierRes = expect(Token::Identifier, "Expected identifier");
	if (!identifierRes.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(identifierRes.error());
	}

	std::unique_ptr<TypeNode> type;
	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(colon.error());
		}

		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if (!typeRes.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(typeRes.error());
		}
		type = std::move(typeRes.value());
	}

	Result<Token> arrow = expect(Token::Arrow, "Expected '->' after variable");
	if (!arrow.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(arrow.error());
	}

	Result<std::unique_ptr<ASTNode>> iterator = parseExpression();
	if (!iterator.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(iterator.error());
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after expression");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<ForStatementNode>(
			keyword.value().position,
			identifierRes.value().value,
			dynamic_unique_cast<ExpressionNode>(std::move(iterator.value())),
			dynamic_unique_cast<StatementNode>(std::move(body.value())),
			std::move(type)
	));
}

Result<std::unique_ptr<ASTNode>> Parser::parseWhileStatement() {
	Result<Token> keywordRes = advance();
	if (!keywordRes.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keywordRes.error());
	}
	Token keyword = keywordRes.value();

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after while statement");
	if (!leftParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftParen.error());
	}

	Token::Position conditionPos = peek().position;
	Result<std::unique_ptr<ASTNode>> condition = parseExpression();
	if (!condition.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(condition.error());
	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after condition");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	Token::Position bodyPos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	Result<std::unique_ptr<ASTNode>> elseStatement;

	if(peek().type == Token::KeywordElse) {
		Result<Token> token = advance();
		if (!token.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(token.error());
		}

		elseStatement = parseStatement();
		if (!elseStatement.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(elseStatement.error());
	}
	}
	return Ok(std::make_unique<WhileStatementNode>(
			keyword.position,
			keyword.value == "dowhile",
			dynamic_unique_cast<ExpressionNode>(std::move(condition.value())),
			dynamic_unique_cast<StatementNode>(std::move(body.value())),
			elseStatement.has_value() ? dynamic_unique_cast<StatementNode>(std::move(elseStatement.value())) : nullptr));
}


Result<std::unique_ptr<ASTNode>> Parser::parseReturnStatement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	bool isSuper = false;
	if(peek().type == Token::KeywordSuper) {
		isSuper = true;
		Result<Token> super = advance();
		if (!super.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(super.error());
		}
	}

	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if (!expr.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(expr.error());
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if (!semiColon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semiColon.error());
	}

	return Ok(std::make_unique<ReturnStatementNode>(token.value().position, dynamic_unique_cast<ExpressionNode>(std::move(expr.value())), isSuper));
}

Result<std::unique_ptr<ASTNode>> Parser::parseBreakStatement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if (!semiColon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semiColon.error());
	}

	return Ok(std::make_unique<BreakStatementNode>(token.value().position));
}

Result<std::unique_ptr<ASTNode>> Parser::parseContinueStatement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if (!semiColon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semiColon.error());
	}

	return Ok(std::make_unique<ContinueStatementNode>(token.value().position));
}

Result<std::unique_ptr<ASTNode>> Parser::parseTypeAlias() {
	Result<Token> typeAlias = advance();
	if (!typeAlias.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(typeAlias.error());
	}

	Result<Token> identifier = expect(Token::Identifier, "Expected identifier after using");
	if (!identifier.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(identifier.error());
	}

	Result<Token> assign = expect(Token::Assign, "Expected '=' after using");
	if (!assign.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(assign.error());
	}

	Token::Position pos = peek().position;
	Result<std::unique_ptr<TypeNode>> type = parseType();
	if (!type.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(type.error());
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';' after type alias");
	if (!semiColon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semiColon.error());
	}

	return Ok(std::make_unique<TypeAliasNode>(
			typeAlias.value().position,
			identifier.value().value,
			std::move(type.value())
		));
}

Result<std::unique_ptr<ASTNode>> Parser::parseClassDeclaration() {
	Result<Token> classKeyword = advance();
	if (!classKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(classKeyword.error());
	}

	Result<Token> className = advance();
	if (!className.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(className.error());
	}

	// Parse generic parameters if present: class Name<T, U> { ... }
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if(peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if (!less.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(less.error());
		}

		// Parse generic parameters
		do {
			Token::Position paramPos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parseGenericParameter();
			if (!param.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(param.error());
			}
			
			genericParams.push_back(std::move(param.value()));

			if(peek().type == Token::Comma) {
				Token::Position commaPos = peek().position;
				Result<Token> comma = advance();
				if (!comma.has_value()) {
					return Err<std::unique_ptr<ASTNode>>(comma.error());
				}
			} else {
				break;
			}
		} while(true);

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if (!greater.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(greater.error());
		}
	}

	std::vector<ConstructorStatementNode> constructors;
	std::vector<ClassDeclarationNode::ClassMember> members;

//  TODO: in the very far future
//	if(peek().type == Token::RightParen) {
//
//	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after class declaration");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	while(peek().type != Token::RightBrace) {
		MemberVisibility visibility = MemberVisibility::PRI;

		if(	peek().type == Token::KeywordPub ||
			peek().type == Token::KeywordPri ||
			peek().type == Token::KeywordPro) {
			Result<Token> visKeyword = advance();
			if (!visKeyword.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(visKeyword.error());
			}

			visibility = 	visKeyword.value().type == Token::KeywordPub ? MemberVisibility::PUB :
							visKeyword.value().type == Token::KeywordPri ? MemberVisibility::PRI :
							MemberVisibility::PRO;

		}

		if(peek().type != Token::KeywordDef &&
		   peek().type != Token::KeywordFunc &&
		   peek().type != Token::KeywordConstructor)
			return Err<std::unique_ptr<ASTNode>>(create_parse_error(ErrorType::UnexpectedToken, "Expected either function declaration, variable declaration or constructor declaration", peek().position));

		Result<std::unique_ptr<ASTNode>> member = parseStatement();
		if (!member.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(member.error());
		}
		Token::Position memberPosition = member.value()->position;
		members.emplace_back(memberPosition, dynamic_unique_cast<StatementNode>(std::move(member.value())), visibility);
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after class declaration");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<ClassDeclarationNode>(
		classKeyword.value().position,
		className.value().value,
		std::move(members),
		std::move(genericParams)
	));
}

Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> Parser::parseConstructorArgument() {
	Result<Token> argIdentifier = expect(Token::Identifier, "Expected identifier in function argument");
	if (!argIdentifier.has_value()) {
		return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(argIdentifier.error());
	}

	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ExpressionNode> value;
	std::string initializes;

	if(peek().type == Token::LeftParen) {
		Result<Token> leftParen = expect(Token::LeftParen, "Expected '('");
		if (!leftParen.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(leftParen.error());
		}

		Result<Token> initIdentifier = expect(Token::Identifier, "Expected identifier");
		if (!initIdentifier.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(initIdentifier.error());
		}
		initializes = initIdentifier.value().value;

		Result<Token> rightParen = expect(Token::RightParen, "Expected ')'");
		if (!rightParen.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(rightParen.error());
		}
	}

	if(peek().type == Token::Colon) {
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(colon.error());
		}

		
		Result<std::unique_ptr<TypeNode>> typeRes = parseType();
		if (!typeRes.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(typeRes.error());
		}

		type = std::move(typeRes.value());
	}

	if(peek().type == Token::Assign) {
		Result<Token> assign = advance();
		if (!assign.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(assign.error());
		}

		
		Result<std::unique_ptr<ASTNode>> valueRes = parseExpression();
		if (!valueRes.has_value()) {
			return Err<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>>(valueRes.error());
		}

		value = dynamic_unique_cast<ExpressionNode>(std::move(valueRes.value()));
	}

	return { std::make_unique<ConstructorStatementNode::ConstructorArgument>(argIdentifier.value().position, argIdentifier.value().value, initializes, std::move(type), std::move(value)) };

}

Result<std::unique_ptr<ASTNode>> Parser::parseConstructorStatement() {
	Result<Token> constructorKeyword = advance();
	if (!constructorKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constructorKeyword.error());
	}

	std::vector<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> args;

	Result<Token> leftParen = expect(Token::LeftParen, "Expected '(' after constructor");
	if (!leftParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftParen.error());
	}

	while(true) {
		Result<std::unique_ptr<ConstructorStatementNode::ConstructorArgument>> arg = parseConstructorArgument();
		if (!arg.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arg.error());
		}

		args.push_back(std::move(arg.value()));

		if(peek().type == Token::RightParen) break;

		Result<Token> comma = expect(Token::Comma, "Expected ',' between arguments");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}

	}

	Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after constructor");
	if (!rightParen.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightParen.error());
	}

	if(peek().type == Token::Semicolon) {
		return Ok(std::make_unique<ConstructorStatementNode>(constructorKeyword.value().position, std::move(args), nullptr));
	}

	
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<ConstructorStatementNode>(constructorKeyword.value().position, std::move(args), std::move(body.value())));
}

Result<std::unique_ptr<ASTNode>> Parser::parseBlock() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	std::vector<std::unique_ptr<ASTNode>> body;
	while(peek().type != Token::RightBrace) {
		Result<std::unique_ptr<ASTNode>> statement = parseStatement();
		if (!statement.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(statement.error());
		}

		if(statement.value()->getNodeGroup() == ASTNodeGroup::Expression) {
			Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
			if (!semiColon.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(semiColon.error());
			}
		}

		body.push_back(std::move(statement.value()));
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after opening '{'");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<BlockNode>(token.value().position, std::move(body)));
}

Result<std::unique_ptr<ASTNode>> Parser::parseImplStatement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	Result<Token> className = expect(Token::Identifier, "Expected Class Name");
	if (!className.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(className.error());
	}

	Result<Token> keyword = advance();
	if (!keyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(keyword.error());
	}

	MemberVisibility visibility = MemberVisibility::PRI;
	if(	keyword.value().type == Token::KeywordPub ||
		keyword.value().type == Token::KeywordPri ||
		keyword.value().type == Token::KeywordPro) {
		visibility = 	keyword.value().type == Token::KeywordPub ? MemberVisibility::PUB :
						keyword.value().type == Token::KeywordPri ? MemberVisibility::PRI :
						MemberVisibility::PRO;

		keyword = advance();
		if (!keyword.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(keyword.error());
		}
	}

	
	if(keyword.value().type != Token::KeywordFunc && keyword.value().type != Token::KeywordDef) {
		return Err<std::unique_ptr<ASTNode>>(create_parse_error(ErrorType::UnexpectedToken, "Expected variable or function declaration", token.value().position));
	}

	Token::Position bodyPos = peek().position;
	Result<std::unique_ptr<ASTNode>> body = parseStatement();
	if (!body.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(body.error());
	}

	return Ok(std::make_unique<ImplStatementNode>(
			token.value().position,
			className.value().value,
			dynamic_unique_cast<StatementNode>(std::move(body.value())), visibility));
}

Result<std::unique_ptr<ASTNode>> Parser::parseYieldStatement() {
	Result<Token> token = advance();
	if (!token.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(token.error());
	}

	
	Result<std::unique_ptr<ASTNode>> expr = parseExpression();
	if (!expr.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(expr.error());
	}

	Result<Token> semiColon = expect(Token::Semicolon, "Expected ';'");
	if (!semiColon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semiColon.error());
	}

	return Ok(std::make_unique<YieldStatementNode>(
					token.value().position,
					dynamic_unique_cast<ExpressionNode>(std::move(expr.value()))
			        ));
}

Result<std::unique_ptr<ASTNode>> Parser::parseEnumDeclaration() {
	Result<Token> enumKeyword = advance();
	if (!enumKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(enumKeyword.error());
	}

	bool isUnion = false;
	if(peek().type == Token::KeywordUnion) {
		isUnion = true;
		Result<Token> unionKeyword = advance();
		if (!unionKeyword.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(unionKeyword.error());
		}
	}

	Result<Token> enumName = expect(Token::Identifier, "Expected enum name");
	if (!enumName.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(enumName.error());
	}

	Result<Token> leftBrace = expect(Token::LeftBrace, "Expected '{' after enum name");
	if (!leftBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(leftBrace.error());
	}

	std::vector<EnumDeclarationNode::EnumVariant> variants;
	while(peek().type != Token::RightBrace) {
		Result<Token> variantName = expect(Token::Identifier, "Expected variant name");
		if (!variantName.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(variantName.error());
		}

		std::vector<std::unique_ptr<TypeNode>> fields;
		if(peek().type == Token::LeftParen) {
			Result<Token> leftParen = advance();
			if (!leftParen.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(leftParen.error());
			}

			while(peek().type != Token::RightParen) {
				Token::Position pos = peek().position;
				Result<std::unique_ptr<TypeNode>> fieldType = parseType();
			if (!fieldType.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(fieldType.error());
			}

			fields.push_back(std::move(fieldType.value()));

				if(peek().type == Token::RightParen) break;
				Result<Token> comma = expect(Token::Comma, "Expected ',' between enum fields");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
			}

			Result<Token> rightParen = expect(Token::RightParen, "Expected ')' after enum fields");
			if (!rightParen.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(rightParen.error());
			}
		}

		variants.emplace_back(variantName.value().position, variantName.value().value, std::move(fields));

		if(peek().type == Token::RightBrace) break;
		Result<Token> comma = expect(Token::Comma, "Expected ',' between enum variants");
		if (!comma.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(comma.error());
		}
	}

	Result<Token> rightBrace = expect(Token::RightBrace, "Expected '}' after enum declaration");
	if (!rightBrace.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(rightBrace.error());
	}

	return Ok(std::make_unique<EnumDeclarationNode>(enumKeyword.value().position, enumName.value().value, std::move(variants), isUnion));
}

Result<std::unique_ptr<ASTNode>> Parser::parseUnionDeclaration() {
	Result<Token> unionKeyword = advance();
	if (!unionKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(unionKeyword.error());
	}

	Result<Token> unionName = expect(Token::Identifier, "Expected union name");
	if (!unionName.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(unionName.error());
	}

	Result<Token> equals = expect(Token::Assign, "Expected '=' after union name");
	if (!equals.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(equals.error());
	}

	std::vector<std::unique_ptr<TypeNode>> types;
	
	// Parse first type
	Token::Position firstTypePos = peek().position;
	Result<std::unique_ptr<TypeNode>> firstType = parseType();
	if (!firstType.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(firstType.error());
	}
	types.push_back(std::move(firstType.value()));

	// Parse additional types separated by |
	while(peek().type == Token::BitwiseOr) {
		Result<Token> pipe = advance();
		if (!pipe.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(pipe.error());
		}

		Token::Position typePos = peek().position;
		Result<std::unique_ptr<TypeNode>> type = parseType();
		if (!type.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(type.error());
		}
		types.push_back(std::move(type.value()));
	}

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after union declaration");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	return Ok(std::make_unique<UnionDeclarationNode>(unionKeyword.value().position, unionName.value().value, std::move(types)));
}

Result<std::unique_ptr<ASTNode>> Parser::parseConstraintDeclaration() {
	Token::Position startPos = peek().position;
	Result<Token> constraintKeyword = advance();
	if (!constraintKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constraintKeyword.error());
	}

	Token::Position namePos = peek().position;
	Result<Token> constraintName = expect(Token::Identifier, "Expected constraint name");
	if (!constraintName.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constraintName.error());
	}

	// Parse generic parameters if present
	std::vector<std::unique_ptr<GenericParameter>> genericParams;
	if(peek().type == Token::Less) {
		Token::Position lessPos = peek().position;
		Result<Token> less = advance();
		if (!less.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(less.error());
		}

		while(peek().type != Token::Greater) {
			Token::Position pos = peek().position;
			Result<std::unique_ptr<GenericParameter>> param = parseGenericParameter();
			if (!param.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(param.error());
			}

			genericParams.push_back(std::move(param.value()));

			if(peek().type == Token::Greater) break;
			Token::Position commaPos = peek().position;
			Result<Token> comma = expect(Token::Comma, "Expected ',' between generic parameters");
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
		}

		Token::Position greaterPos = peek().position;
		Result<Token> greater = expect(Token::Greater, "Expected '>' after generic parameters");
		if (!greater.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(greater.error());
		}
	}

	Token::Position assignPos = peek().position;
	Result<Token> assign = expect(Token::Assign, "Expected '=' after constraint declaration");
	if (!assign.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(assign.error());
	}

	Token::Position exprPos = peek().position;
	Result<std::unique_ptr<ASTNode>> constraintExpressionRes = parseExpression();
	if (!constraintExpressionRes.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(constraintExpressionRes.error());
	}
	
	std::unique_ptr<ExpressionNode> constraintExpression = dynamic_unique_cast<ExpressionNode>(std::move(constraintExpressionRes.value()));

	Token::Position semicolonPos = peek().position;
	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after constraint expression");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	return Ok(std::make_unique<ConstraintDeclarationNode>(constraintKeyword.value().position, constraintName.value().value, std::move(genericParams), std::move(constraintExpression)));
}

Result<std::unique_ptr<GenericParameter>> Parser::parseGenericParameter() {
	Token::Position namePos = peek().position;
	Result<Token> name = expect(Token::Identifier, "Expected generic parameter name");
	if (!name.has_value()) {
		return Err<std::unique_ptr<GenericParameter>>(name.error());
	}

	std::unique_ptr<TypeNode> constraint = nullptr;
	
	// Check for constraint: T: Number
	if(peek().type == Token::Colon) {
		Token::Position colonPos = peek().position;
		Result<Token> colon = advance();
		if (!colon.has_value()) {
			return Err<std::unique_ptr<GenericParameter>>(colon.error());
		}

		Token::Position constraintPos = peek().position;
		Result<std::unique_ptr<TypeNode>> constraintResult = parseType();
		if (!constraintResult.has_value()) {
			return Err<std::unique_ptr<GenericParameter>>(constraintResult.error());
		}

		constraint = std::move(constraintResult.value());
	}

	return Ok(std::make_unique<GenericParameter>(namePos, name.value().value, std::move(constraint)));
}

Result<std::unique_ptr<ASTNode>> Parser::parseModuleDeclaration() {
	Result<Token> moduleKeyword = advance();
	if (!moduleKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(moduleKeyword.error());
	}

	Result<Token> moduleName = expect(Token::Identifier, "Expected module name");
	if (!moduleName.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(moduleName.error());
	}

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after module declaration");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	// For now, we'll create a simple module declaration without body
	// In a full implementation, we'd parse the module body
	return Ok(std::make_unique<ModuleDeclarationNode>(moduleKeyword.value().position, moduleName.value().value, std::vector<std::unique_ptr<StatementNode>>()));
}

Result<std::unique_ptr<ASTNode>> Parser::parseImportStatement() {
	Result<Token> importKeyword = advance();
	if (!importKeyword.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(importKeyword.error());
	}

	Result<Token> moduleName = expect(Token::Identifier, "Expected module name");
	if (!moduleName.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(moduleName.error());
	}

	std::vector<std::string> importedItems;
	if(peek().type == Token::Arrow) {
		Result<Token> arrow = advance();
		if (!arrow.has_value()) {
			return Err<std::unique_ptr<ASTNode>>(arrow.error());
		}

		// Parse imported items
		do {
			Result<Token> item = expect(Token::Identifier, "Expected imported item name");
			if (!item.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(item.error());
			}

			importedItems.push_back(item.value().value);

			if(peek().type == Token::Comma) {
				Result<Token> comma = advance();
			if (!comma.has_value()) {
				return Err<std::unique_ptr<ASTNode>>(comma.error());
			}
			} else {
				break;
			}
		} while(true);
	}

	Result<Token> semicolon = expect(Token::Semicolon, "Expected ';' after import statement");
	if (!semicolon.has_value()) {
		return Err<std::unique_ptr<ASTNode>>(semicolon.error());
	}

	return Ok(std::make_unique<ImportStatementNode>(importKeyword.value().position, moduleName.value().value, std::move(importedItems)));
}

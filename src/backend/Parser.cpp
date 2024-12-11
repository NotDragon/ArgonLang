#include "backend/Parser.h"
#include <stdexcept>

template <typename Target, typename Source>
std::unique_ptr<Target> dynamic_unique_cast(std::unique_ptr<Source> source) {
	auto* result = dynamic_cast<Target*>(source.get());
	if (!result) {
		throw std::runtime_error("Invalid cast");
	}
	source.release();
	return std::unique_ptr<Target>(result);
}

const ArgonLang::Token& ArgonLang::Parser::peek() const {
    if (current >= tokens.size()) {
        throw std::runtime_error("Unexpected end of input");
    }
    return tokens[current];
}

bool ArgonLang::Parser::eos() const {
	return current >= tokens.size() - 1;
}

const ArgonLang::Token& ArgonLang::Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
    throw std::runtime_error("Unexpected end of input");
}

ArgonLang::Token ArgonLang::Parser::expect(Token::Type type, const std::string& errorMessage) {
    if (current >= tokens.size() || tokens[current].type != type) {
        throw std::runtime_error(errorMessage);
    }
	return advance();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parsePrimary() {
    const Token& token = advance();

    if (token.type == Token::IntegralLiteral) {
        return std::make_unique<IntegralLiteralNode>(std::stoi(token.value), determineIntegerType(token.value)); // stoi needs to be changed
    } else if (token.type == Token::FloatLiteral) {
        return std::make_unique<FloatLiteralNode>(std::stod(token.value), determineFloatType(token.value)); // stod needs to be changed
    } else if (token.type == Token::StringLiteral) {
        return std::make_unique<StringLiteralNode>(token.value);
    } else if (token.type == Token::BooleanLiteral) {
        return std::make_unique<BooleanLiteralNode>(token.value == "true");
    } else if (token.type == Token::Identifier) {
        return std::make_unique<IdentifierNode>(token.value);
    } else if (token.type == Token::LeftParen) {
        auto expr = parseExpression();
        expect(Token::RightParen, "Expected closing ')'");
        return expr;
    }

    throw std::runtime_error("Unexpected token: " + token.value + " " + Token::getTypeAsString(token.type));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseAdditiveExpression() {
    std::unique_ptr<ArgonLang::ASTNode> left = parseMultiplicativeExpression();

    while(peek().type == Token::Plus || peek().type == Token::Minus) {
        const Token op = advance();
        std::unique_ptr<ArgonLang::ASTNode> right = parseMultiplicativeExpression();

       left = std::make_unique<BinaryExpressionNode>(
			   dynamic_unique_cast<ExpressionNode>(std::move(left)),
			   op,
			   dynamic_unique_cast<ExpressionNode>(std::move(right))
            );
    }

    return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseMultiplicativeExpression() {
    std::unique_ptr<ArgonLang::ASTNode> left = parseBitwiseNotExpression();

    while(peek().type == Token::Multiply || peek().type == Token::Divide || peek().type == Token::Modulo) {
        const Token op = advance();
        std::unique_ptr<ArgonLang::ASTNode> right = parseBitwiseNotExpression();

        left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
            );
    }

    return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseExpression() {
    return parseAssignmentExpression();
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parse() {
	std::vector<std::unique_ptr<ASTNode>> statements;
	while(!eos()) {
		statements.push_back(parseStatement());
	}
	return std::make_unique<ProgramNode>(std::move(statements));
}

ArgonLang::Parser::Parser(const std::vector<Token> &tokens)  : tokens(tokens) {}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseLogicalOrExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseLogicalAndExpression();

	while(peek().type == Token::LogicalOr) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseLogicalAndExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseLogicalAndExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseEqualityExpression();

	while(peek().type == Token::LogicalAnd) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseEqualityExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseEqualityExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseRelationalExpression();

	while(peek().type == Token::Equal) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseRelationalExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseRelationalExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseBitwiseExpression();

	while(peek().type == Token::Greater || peek().type == Token::GreaterEqual || peek().type == Token::Less || peek().type == Token::LessEqual) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseBitwiseExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseBitwiseExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseShiftExpression();

	while(peek().type == Token::BitwiseOr || peek().type == Token::BitwiseAnd || peek().type == Token::BitwiseXor) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseShiftExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseShiftExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseToExpression();

	while(peek().type == Token::LeftShift || peek().type == Token::RightShift) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseToExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseAssignmentExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseParallelExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseMemberAccessExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parsePrimary();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseIndexingExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseMemberAccessExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseFilterExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseMapExpression();

	while(peek().type == Token::FilterRange) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseMapExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseMapExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseReduceExpression();

	while(peek().type == Token::MapRange) {
		const Token op = advance();
		std::unique_ptr<ArgonLang::ASTNode> right = parseReduceExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseReduceExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseLogicalOrExpression();

	while(peek().type == Token::ReduceRange) {
		const Token op = advance();

		std::unique_ptr<ArgonLang::ASTNode> right = parseLogicalOrExpression();

		left = std::make_unique<BinaryExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				op,
				dynamic_unique_cast<ExpressionNode>(std::move(right))
		);
	}

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseRangeExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseFunctionCallExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseFunctionCallExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseIndexingExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseToExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseAdditiveExpression();

	while(peek().type == Token::KeywordTo) {
		bool isInclusive = advance().type == Token::Assign;
		std::unique_ptr<ArgonLang::ASTNode> right = parseAdditiveExpression();

		left = std::make_unique<ToExpressionNode>(
				dynamic_unique_cast<ExpressionNode>(std::move(left)),
				dynamic_unique_cast<ExpressionNode>(std::move(right)),
				isInclusive
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseDerefExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseRangeExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseReferenceExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseDerefExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseIncrementExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseReferenceExpression();

	while(peek().type == Token::Increment) {
		left = std::make_unique<UnaryExpressionNode>(
				advance(),
				dynamic_unique_cast<ExpressionNode>(std::move(left))
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseUnaryMinusExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseIncrementExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseUnaryPlusExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseUnaryMinusExpression();

	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseLogicalNotExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseUnaryPlusExpression();

	while(peek().type == Token::LogicalNot) {
		left = std::make_unique<UnaryExpressionNode>(
				advance(),
				dynamic_unique_cast<ExpressionNode>(std::move(left))
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseBitwiseNotExpression() {
	std::unique_ptr<ArgonLang::ASTNode> left = parseLogicalNotExpression();

	while(peek().type == Token::BitwiseNot) {
		left = std::make_unique<UnaryExpressionNode>(
				advance(),
				dynamic_unique_cast<ExpressionNode>(std::move(left))
		);
	}
	return left;
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseParallelExpression() {
	if(peek().type != Token::KeywordParallel) return parseAwaitExpression();
	advance();
	std::unique_ptr<ArgonLang::ASTNode> left = parseStatement();

	return std::make_unique<ParallelExpressionNode>(std::move(left));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseAwaitExpression() {
	if(peek().type != Token::KeywordAwait) return parseLazyExpression();
	advance();
	std::unique_ptr<ArgonLang::ASTNode> left = parseStatement();

	return std::make_unique<AwaitExpressionNode>(dynamic_unique_cast<ExpressionNode>(std::move(left)));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseLazyExpression() {
	if(peek().type != Token::KeywordLazy) return parseStructExpression();
	advance();
	std::unique_ptr<ArgonLang::ASTNode> left = parseStatement();

	return std::make_unique<LazyExpressionNode>(dynamic_unique_cast<ExpressionNode>(std::move(left)));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseStructExpression() {
	if(peek().type != Token::KeywordStruct) return parseMatchExpression();
	advance();
	expect(Token::LeftBrace, "Expected '{' after struct");

	std::vector<std::tuple<std::string, std::unique_ptr<TypeNode>, std::unique_ptr<ExpressionNode>>> fields;
	while(peek().type != Token::RightBrace) {
		Token name = expect(Token::Identifier, "Expected fields name");
		std::unique_ptr<TypeNode> type;
		std::unique_ptr<ExpressionNode> expression;
		if(peek().type == Token::Colon) {
			advance();
			type = parseType();
		}
		if(peek().type == Token::Equal) {
			advance();
			expression = dynamic_unique_cast<ExpressionNode>(parseExpression());
		}
		if(!type && !expression) {
			throw std::runtime_error("Cannot have field without value or type");
		}
		fields.emplace_back(name.value, std::move(type), std::move(expression));
	}

	return std::make_unique<StructExpressionNode>(std::move(fields));
}

std::unique_ptr<ArgonLang::ASTNode> ArgonLang::Parser::parseMatchExpression() {
	if(peek().type != Token::KeywordMatch) return parseFilterExpression();
	advance();
	expect(Token::LeftParen, "Expected '(' after match");
	std::unique_ptr<ExpressionNode> value = dynamic_unique_cast<ExpressionNode>(parseExpression());
	expect(Token::RightParen, "Expected ')' after identifier");
	expect(Token::LeftBrace, "Expected '{' after match expression");
	std::vector<std::unique_ptr<MatchBranch>> branches;
	while(peek().type != Token::RightBrace) {
		std::unique_ptr<ExpressionNode> pattern = dynamic_unique_cast<ExpressionNode>(parseExpression());
		expect(Token::Arrow, "Expected '->' after pattern");
		std::unique_ptr<ExpressionNode> body = dynamic_unique_cast<ExpressionNode>(parseExpression());
		branches.push_back(std::make_unique<MatchBranch>(std::move(pattern), nullptr, std::move(body)));
		if(peek().type == Token::RightBrace) break;
		expect(Token::Comma, "Expected ',' or '}'");
	}
	expect(Token::RightBrace, "Expected '}' after match statement");

	return std::make_unique<MatchExpressionNode>(std::move(value), std::move(branches));
}


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

std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseGenericType() {
	return std::unique_ptr<TypeNode>();
}

std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseUnionType() {
	return std::unique_ptr<TypeNode>();
}

std::unique_ptr<ArgonLang::TypeNode> ArgonLang::Parser::parseArrayType() {
	return std::unique_ptr<TypeNode>();
}







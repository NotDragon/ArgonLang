#include "backend/Parser.h"
#include <stdexcept>

using namespace ArgonLang;

Token Parser::peek() const {
	return tokens[current];
}

bool Parser::eos() const {
	return current >= tokens.size() - 1;
}

Result<Token> Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
	return { "Unexpected end of input", Token() };
}

Result<std::unique_ptr<ProgramNode>> Parser::parse() {
	std::vector<std::unique_ptr<ASTNode>> statements;
	while(!eos()) {
		Result<std::unique_ptr<ASTNode>> statement;
		switch (peek().type) {
			case Token::KeywordDef:
				statement = parseVariableDeclaration();
				break;
			case Token::KeywordFunc:
				statement = parseFunctionDeclaration();
				break;
			default:
				return { "Only Function and Variable declarations are allowed in the outer scope" };
		}

		if(statement.hasError()) return { statement.getErrorMsg() };

		statements.push_back(statement.moveValue());
	}
	return std::make_unique<ProgramNode>(std::move(statements));
}

Result<Token> Parser::expect(Token::Type type, const std::string& errorMessage) {
    if (current >= tokens.size() || tokens[current].type != type) {
        return { errorMessage, Token() };
    }
	return advance();
}

int Parser::getMainCounter() {
	return mainCounter;
}
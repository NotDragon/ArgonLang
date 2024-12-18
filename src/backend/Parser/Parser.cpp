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

Result<Token> Parser::expect(Token::Type type, const std::string& errorMessage) {
    if (current >= tokens.size() || tokens[current].type != type) {
        return { errorMessage, Token() };
    }
	return advance();
}
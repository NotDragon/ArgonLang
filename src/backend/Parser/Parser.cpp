#include "backend/Parser.h"
#include <stdexcept>

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
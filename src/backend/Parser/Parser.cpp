#include "backend/Parser.h"
#include <stdexcept>

using namespace ArgonLang;

Token Parser::peek() const {
	return tokens[current];
}

Token Parser::peek(int offset) const {
	size_t index = current + offset;
	if(index >= tokens.size()) {
		return tokens.back(); // Return end token if out of bounds
	}
	return tokens[index];
}

bool Parser::eos() const {
	return current >= tokens.size() - 1;
}

[[nodiscard]] Result<Token> Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
	return { "Unexpected end of input", "", peek() };
}

[[nodiscard]] Result<Token> Parser::expect(Token::Type type, const std::string& errorMessage) {
    if (current >= tokens.size() || tokens[current].type != type) {
        // Don't modify parser state on error - create error with current position info
        Token errorToken = (current < tokens.size()) ? tokens[current] : tokens.back();
        std::string fullErrorMsg = ErrorFormatter::formatParseError(
            errorMessage, 
            Token::getTypeAsString(type), 
            errorToken
        );
        std::string note = ErrorFormatter::createSuggestion(
            "Check syntax near " + ErrorFormatter::formatPosition(errorToken.position)
        );
        return { fullErrorMsg, note, errorToken };
    }
	return advance();
}

int Parser::getMainCounter() const {
	return mainCounter;
}

Result<std::unique_ptr<ProgramNode>> Parser::parse() {
	std::vector<std::unique_ptr<ASTNode>> statements;
	while(!eos()) {
		Result<std::unique_ptr<ASTNode>> statement;
		size_t start = current;
		switch (peek().type) {
			case Token::KeywordDef:
				statement = parseVariableDeclaration();
				break;
			case Token::KeywordFunc:
				statement = parseFunctionDeclaration();
				break;
			case Token::KeywordModule:
				statement = parseModuleDeclaration();
				break;
			case Token::KeywordImport:
				statement = parseImportStatement();
				break;
			case Token::KeywordUsing:
				statement = parseTypeAlias();
				break;
			case Token::KeywordEnum:
				statement = parseEnumDeclaration();
				break;
			case Token::KeywordConstraint:
				statement = parseConstraintDeclaration();
				break;
			case Token::KeywordClass:
				statement = parseClassDeclaration();
				break;
			default:
				Token currentToken = peek();
				std::string errorMsg = ErrorFormatter::formatParseError(
					"Invalid declaration at top level",
					"function, variable, module, import, type alias, enum, or class declaration",
					currentToken
				);
				std::string note = ErrorFormatter::createContext("Only declarations are allowed at the top level");
				return { errorMsg, note, Trace(ASTNodeType::Program, currentToken.position) };
		}

		if(statement.hasError()) {
			synchronize();
			// Propagate error with proper trace information
			return { statement, Trace(ASTNodeType::Program, peek().position) };
		}

		statements.push_back(statement.moveValue());
	}
	return std::make_unique<ProgramNode>(Token::Position(0, 0), std::move(statements));
}

void Parser::synchronize() {
	while (!eos()) {
		Token currentType = peek();

		if (currentType.type == Token::Semicolon ||
			currentType.type == Token::RightBrace) {
			// Consume the synchronization token and return
			(void)advance(); // Cast to void to suppress nodiscard warning
			return;
		}
		(void)advance(); // Cast to void to suppress nodiscard warning
	}
}

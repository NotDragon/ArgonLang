#include "backend/Parser.h"
#include <stdexcept>

using namespace ArgonLang;

Token Parser::peek() const {
	return tokens[current];
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
    if (current > tokens.size() || tokens[current].type != type) {
		current--;
        return { errorMessage + " got " + peek().value + "(" + Token::getTypeAsString(peek().type) + ")", "", peek() };
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
			case Token::KeywordTrait:
				statement = parseTraitDeclaration();
				break;
			case Token::KeywordClass:
				statement = parseClassDeclaration();
				break;
			default:
				return { "Only Function, Variable, Module, Import, Type Alias, Enum, Trait, and Class declarations are allowed in the outer scope, but got " + peek().value + "(" + Token::getTypeAsString(peek().type) + ")", "", Trace(ASTNodeType::Program, peek().position) };
		}

		if(statement.hasError()) {
			synchronize();

			return { std::move(statement), Trace(ASTNodeType::Program, { 0, 0 }) };
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
			advance();
			return;
		}
		advance();
	}
}

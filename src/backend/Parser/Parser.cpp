#include "backend/Parser.h"

#include "Error/Error.h"
#include "Error/Result.h"

#include <stdexcept>

using namespace ArgonLang;

Token Parser::peek() const {
	return tokens[current];
}

Token Parser::peek(int offset) const {
	size_t index = current + offset;
	if (index >= tokens.size()) {
		return tokens.back(); // Return end token if out of bounds
	}
	return tokens[index];
}

bool Parser::eos() const {
	return current >= tokens.size() - 1;
}

[[nodiscard]] Result<Token> Parser::advance() {
	if (current < tokens.size()) {
		return Ok(tokens[current++]);
	}
	Token endToken = peek();
	Error error = create_parse_error(ErrorType::UnexpectedToken, "Unexpected end of input", endToken.position);
	error.withExpected("more tokens").withActual("end of input");
	return Err<Token>(error);
}

[[nodiscard]] Result<Token> Parser::expect(Token::Type type, const std::string& errorMessage) {
	if (current >= tokens.size() || tokens[current].type != type) {
		// Don't modify parser state on error - create error with current position info
		Token errorToken = (current < tokens.size()) ? tokens[current] : tokens.back();
		Position pos("", errorToken.position.line, errorToken.position.column);

		Error error = create_parse_error(ErrorType::MissingToken, errorMessage, errorToken.position);
		error.withExpected(Token::getTypeAsString(type))
		    .withActual(errorToken.value)
		    .withSuggestion("Check syntax near line " + std::to_string(errorToken.position.line) + ", column " +
		                    std::to_string(errorToken.position.column));
		return Err<Token>(error);
	}
	return advance();
}

int Parser::get_main_counter() const {
	return main_counter;
}

Result<std::unique_ptr<ProgramNode>> Parser::parse() {
	std::vector<std::unique_ptr<ASTNode>> statements;
	while (!eos()) {
		Result<std::unique_ptr<ASTNode>> statement;
		size_t start = current;
		switch (peek().type) {
		case Token::KeywordDef:
			statement = parse_variable_declaration();
			break;
		case Token::KeywordFunc:
			statement = parse_function_declaration();
			break;
		case Token::KeywordModule:
			statement = parse_module_declaration();
			break;
		case Token::KeywordImport:
			statement = parse_import_statement();
			break;
		case Token::KeywordUsing:
			statement = parse_type_alias();
			break;
		case Token::KeywordEnum:
			statement = parse_enum_declaration();
			break;
		case Token::KeywordUnion:
			statement = parse_union_declaration();
			break;
		case Token::KeywordConstraint:
			statement = parse_constraint_declaration();
			break;
		case Token::KeywordClass:
			statement = parse_class_declaration();
			break;
		default:
			Token currentToken = peek();
			Error error = create_parse_error(ErrorType::UnexpectedToken, "Invalid declaration at top level",
			                                 currentToken.position);
			error.withExpected("function, variable, module, import, type alias, enum, or class declaration")
			    .withActual(currentToken.value)
			    .with_note("Only declarations are allowed at the top level");
			return Err<std::unique_ptr<ProgramNode>>(error);
		}

		if (!statement.has_value()) {
			synchronize();
			// Propagate error with proper trace information
			return Err<std::unique_ptr<ProgramNode>>(statement.error());
		}

		statements.push_back(std::move(statement.value()));
	}
	return std::make_unique<ProgramNode>(Token::Position(0, 0), std::move(statements));
}

void Parser::synchronize() {
	while (!eos()) {
		Token currentType = peek();

		if (currentType.type == Token::Semicolon || currentType.type == Token::RightBrace) {
			// Consume the synchronization token and return
			(void)advance(); // Cast to void to suppress nodiscard warning
			return;
		}
		(void)advance(); // Cast to void to suppress nodiscard warning
	}
}

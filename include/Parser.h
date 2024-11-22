#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include <vector>
#include <memory>
#include "Tokenizer.h"

class Parser {
private:
    const std::vector<Token>& tokens;
    size_t current = 0; 

    const Token& peek() const;
    const Token& advance();

    bool match(Token::Type type);
    void expect(Token::Type type, const std::string& errorMessage);

    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseAdditiveExpression();
    std::unique_ptr<ASTNode> parseMultiplicativeExpression();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    std::unique_ptr<ASTNode> parse();

};

#endif // PARSER_H

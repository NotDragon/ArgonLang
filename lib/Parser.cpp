#include "Parser.h"
#include <stdexcept>

const Token& Parser::peek() const {
    if (current >= tokens.size()) {
        throw std::runtime_error("Unexpected end of input");
    }
    return tokens[current];
}

const Token& Parser::advance() {
    if (current < tokens.size()) {
        return tokens[current++];
    }
    throw std::runtime_error("Unexpected end of input");
}

bool Parser::match(Token::Type type) {
    if (current < tokens.size() && tokens[current].type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(Token::Type type, const std::string& errorMessage) {
    if (!match(type)) {
        throw std::runtime_error(errorMessage);
    }
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    const Token& token = peek();

    if (match(Token::IntegralLiteral)) {
        return std::make_unique<IntegralLiteralNode>(std::stoi(token.value));
    } else if (match(Token::DecimalLiteral)) {
        return std::make_unique<DecimalLiteralNode>(std::stod(token.value));
    } else if (match(Token::StringLiteral)) {
        return std::make_unique<StringLiteralNode>(token.value);
    } else if (match(Token::BooleanLiteral)) {
        return std::make_unique<BooleanLiteralNode>(token.value == "true");
    } else if (match(Token::Identifier)) {
        return std::make_unique<IdentifierNode>(token.value);
    } else if (match(Token::LeftParen)) {
        auto expr = parseExpression();
        expect(Token::RightParen, "Expected closing ')'");
        return expr;
    }

    throw std::runtime_error("Unexpected token: " + token.value);
}

std::unique_ptr<ASTNode> Parser::parseAdditiveExpression() {
    std::unique_ptr<ASTNode> left = parseMultiplicativeExpression();

    while(peek().type == Token::Plus || peek().type == Token::Minus) {
        const Token op = advance();
        std::unique_ptr<ASTNode> right = parseMultiplicativeExpression();

       left = std::make_unique<BinaryExpressionNode>(
                std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(left.release())),
                op,
                std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(right.release()))
            );
    } 

    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicativeExpression() {
    std::unique_ptr<ASTNode> left = parsePrimary();

    while(peek().type == Token::Multiply || peek().type == Token::Divide || peek().type == Token::Modulo) {
        const Token op = advance();
        std::unique_ptr<ASTNode> right = parsePrimary();

        left = std::make_unique<BinaryExpressionNode>(
                std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(left.release())),
                op,
                std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(right.release()))
            );
    } 

    return left;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseAdditiveExpression();
}

std::unique_ptr<ASTNode> Parser::parse() {
    return parseExpression();
}

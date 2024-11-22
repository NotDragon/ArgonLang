#ifndef AST_H
#define AST_H

#include <string>
#include <iostream>
#include <memory>
#include "Tokenizer.h"
#include "Value.h"

#define DEBUG

class ASTNode {
public:
    virtual ~ASTNode() = default;

#ifdef DEBUG
    virtual void print() const = 0; 
    virtual void toDot(std::ostream& os, int& nodeId) const = 0;
#endif
};

class ExpressionNode : public ASTNode { };

class StatementNode : public ASTNode { };

class StringLiteralNode : public ExpressionNode {
public:
    std::string value;

    explicit StringLiteralNode(const std::string& val);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif
};

class IntegralLiteralNode : public ExpressionNode {
public:
    int value;
    explicit IntegralLiteralNode(int val);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif
};

class DecimalLiteralNode : public ExpressionNode {
public:
    double value;

    explicit DecimalLiteralNode(double val);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif
};

class BooleanLiteralNode : public ExpressionNode {
public:
    bool value;
    explicit BooleanLiteralNode(bool val);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif
};

class IdentifierNode : public ExpressionNode {
public:
    std::string identifier;

    explicit IdentifierNode(const std::string& val);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif

};

class UnaryExpressionNode : public ASTNode {
public:
    Token op;
    std::unique_ptr<ASTNode> operand;

    UnaryExpressionNode(Token operatorSymbol, std::unique_ptr<ASTNode> operandNode);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif
};


class BinaryExpressionNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
    Token op;

    BinaryExpressionNode(std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs);

#ifdef DEBUG
    void print() const override;
    void toDot(std::ostream& os, int& nodeId) const override;
#endif
};

#endif // AST_H
#ifndef PRINTVISITOR_H
#define PRINTVISITOR_H

#include "Visitor.h"
#include "Value.h"
#include <iostream>

class PrintVisitor : public Visitor {
public:
    std::unique_ptr<Value> visit(const BinaryExpressionNode& node) override;
    std::unique_ptr<Value> visit(const IntegralLiteralNode& node) override;
    std::unique_ptr<Value> visit(const DecimalLiteralNode& node) override;
    std::unique_ptr<Value> visit(const StringLiteralNode& node) override;
    std::unique_ptr<Value> visit(const BooleanLiteralNode& node) override;
    std::unique_ptr<Value> visit(const IdentifierNode& node) override;
    std::unique_ptr<Value> visit(const std::unique_ptr<ExpressionNode>& expression) override;
    std::unique_ptr<Value> visit(std::unique_ptr<StatementNode>& statement) override;
};

#endif // PRINTVISITOR_H

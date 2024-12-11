#include "frontend/PrintVisitor.h"
#include "frontend/Value.h"
#include <iostream>

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const BinaryExpressionNode& node) {
    std::cout << "BinaryExpression(" << node.op.value << ")" << std::endl;

    std::cout << "  Left: ";
    if (node.left) {
        visit(node.left);
    } else {
        std::cout << "null" << std::endl;
    }

    std::cout << "  Right: ";
    if (node.right) {
        visit(node.right);
    } else {
        std::cout << "null" << std::endl;
    }

    return nullptr;
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const IntegralLiteralNode &node) {
	return nullptr;
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const FloatLiteralNode &node) {
	return nullptr;
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const StringLiteralNode &node) {
    std::cout << node.value;
	return nullptr;
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const BooleanLiteralNode &node) {
    std::cout << node.value;
	return nullptr;
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const IdentifierNode &node) {
    std::cout << node.identifier;
	return nullptr;
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(const std::unique_ptr<ExpressionNode>& expression) {
    if (auto* node = dynamic_cast<BinaryExpressionNode*>(expression.get())) {
        return visit(*node);
    } else if (auto* node = dynamic_cast<IntegralLiteralNode*>(expression.get())) {
        return visit(*node);
    } else if (auto* node = dynamic_cast<FloatLiteralNode*>(expression.get())) {
        return visit(*node);
    } else if (auto* node = dynamic_cast<StringLiteralNode*>(expression.get())) {
        return visit(*node);
    } else if (auto* node = dynamic_cast<BooleanLiteralNode*>(expression.get())) {
        return visit(*node);
    } else if (auto* node = dynamic_cast<IdentifierNode*>(expression.get())) {
        return visit(*node);
    } else {
        throw std::runtime_error("Unknown Expression");
    }
}

std::unique_ptr<Value> ArgonLang::PrintVisitor::visit(std::unique_ptr<StatementNode>& statement) {
    std::cout << "Visiting StatementNode (not yet implemented)" << std::endl;
    return nullptr;
}

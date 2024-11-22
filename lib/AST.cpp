#include "AST.h"
      
#ifdef DEBUG
    void StringLiteralNode::print() const {
        std::cout << "StringLiteral(" << value << ")";
    }
	void IntegralLiteralNode::print() const {
		std::cout << "IntegralLiteral(" << value << ")";
	}
	void DecimalLiteralNode::print() const {
        std::cout << "DecimalLiteral(" << value << ")";
    }
    void BooleanLiteralNode::print() const {
        std::cout << "BooleanLiteral(" << value << ")";
    }
	
	void IdentifierNode::print() const {
		std::cout << "Identifier(" << identifier << ")";
	}

	void BinaryExpressionNode::print() const {
        std::cout << "BinaryOp(";
        left->print();
        std::cout << " " << op.value << " ";
        right->print();
        std::cout << ")";
    }

	void UnaryExpressionNode::print() const {
		std::cout << "UnaryOp(" << op.value << " ";
        operand->print();
        std::cout << ")";
	}

	void StringLiteralNode::toDot(std::ostream &os, int &nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"StringLiteral(" << value << ")\"];\n";
        
	}
    void IntegralLiteralNode::toDot(std::ostream& os, int& nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"IntegralLiteral(" << value << ")\"];\n";
    }

    void DecimalLiteralNode::toDot(std::ostream& os, int& nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"DecimalLiteral(" << value << ")\"];\n";
    }

    void IdentifierNode::toDot(std::ostream &os, int &nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"Identifier(" << identifier << ")\"];\n";
    }

    void UnaryExpressionNode::toDot(std::ostream &os, int &nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"UnaryExpressionNode(" << op.value << ")\"];\n";
        if(operand) {
            int leftId = nodeId;
            operand->toDot(os, nodeId);
            os << "  node" << currentId << " -> node" << leftId << ";\n";
        }
	}

    void BooleanLiteralNode::toDot(std::ostream &os, int &nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"BooleanLiteral(" << value << ")\"];\n";
	}

    void BinaryExpressionNode::toDot(std::ostream& os, int& nodeId) const {
        int currentId = nodeId++;
        os << "  node" << currentId << " [label=\"BinaryExpression(" << op.value << ")\"];\n";

        if (left) {
            int leftId = nodeId;
            left->toDot(os, nodeId);
            os << "  node" << currentId << " -> node" << leftId << ";\n";
        }
        if (right) {
            int rightId = nodeId;
            right->toDot(os, nodeId);
            os << "  node" << currentId << " -> node" << rightId << ";\n";
        }
}

#endif

StringLiteralNode::StringLiteralNode(const std::string& val) : value(val) {}
IntegralLiteralNode::IntegralLiteralNode(int val) : value(val) {}
DecimalLiteralNode::DecimalLiteralNode(double val) : value(val) {}
BooleanLiteralNode::BooleanLiteralNode(bool val) : value(val) {}
IdentifierNode::IdentifierNode(const std::string& val) : identifier(val) {}
BinaryExpressionNode::BinaryExpressionNode(std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs)
        : left(std::move(lhs)), op(operatorSymbol), right(std::move(rhs)) {}
UnaryExpressionNode::UnaryExpressionNode(
		Token operatorSymbol, std::unique_ptr<ASTNode> operandNode): op(operatorSymbol), operand(std::move(operandNode)) {}


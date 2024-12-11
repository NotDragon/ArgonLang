#ifndef VISITOR_H
#define VISITOR_H
#include "Value.h"
#include "backend/AST.h"

namespace ArgonLang {

	class Visitor {
	public:
		virtual ~Visitor() = default;


		virtual std::unique_ptr<Value> visit(const BinaryExpressionNode &node) = 0;

		virtual std::unique_ptr<Value> visit(const IntegralLiteralNode &node) = 0;

		virtual std::unique_ptr<Value> visit(const FloatLiteralNode &node) = 0;

		virtual std::unique_ptr<Value> visit(const StringLiteralNode &node) = 0;

		virtual std::unique_ptr<Value> visit(const BooleanLiteralNode &node) = 0;

		virtual std::unique_ptr<Value> visit(const IdentifierNode &node) = 0;

		virtual std::unique_ptr<Value> visit(const std::unique_ptr<ExpressionNode> &expression) = 0;

		virtual std::unique_ptr<Value> visit(std::unique_ptr<StatementNode> &statement) = 0;
	};
}

#endif // VISITOR_H
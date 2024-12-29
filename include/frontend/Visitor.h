#ifndef VISITOR_H
#define VISITOR_H
#include "backend/AST.h"

namespace ArgonLang {
	template<typename T>
	class Visitor {
	public:
		virtual ~Visitor() = default;


		virtual T visit(const BinaryExpressionNode &node) = 0;

		virtual T visit(const IntegralLiteralNode &node) = 0;

		virtual T visit(const FloatLiteralNode &node) = 0;

		virtual T visit(const StringLiteralNode &node) = 0;

		virtual T visit(const BooleanLiteralNode &node) = 0;

		virtual T visit(const IdentifierNode &node) = 0;

		virtual T visit(const std::unique_ptr<ExpressionNode> &expression) = 0;

		virtual T visit(const std::unique_ptr<StatementNode> &statement) = 0;
	};
}

#endif // VISITOR_H
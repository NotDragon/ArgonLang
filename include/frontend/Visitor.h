#ifndef VISITOR_H
#define VISITOR_H
#include "backend/AST.h"

namespace ArgonLang {
	template<typename T>
	class Visitor {
	public:
		virtual ~Visitor() = default;

		virtual T visit(const ASTNode& node) = 0;
		virtual T visit(const ExpressionNode& node) = 0;
		virtual T visit(const StatementNode& node) = 0;
		virtual T visit(const TypeNode& node) = 0;

		virtual T visit(const ProgramNode &node) = 0;

		virtual T visit(const IntegralLiteralNode &node) = 0;
		virtual T visit(const FloatLiteralNode &node) = 0;
		virtual T visit(const StringLiteralNode &node) = 0;
		virtual T visit(const CharLiteralNode &node) = 0;
		virtual T visit(const BooleanLiteralNode &node) = 0;
		virtual T visit(const IdentifierNode &node) = 0;

		virtual T visit(const BinaryExpressionNode &node) = 0;
		virtual T visit(const UnaryExpressionNode &node) = 0;

		virtual T visit(const FunctionCallExpressionNode &node) = 0;
		virtual T visit(const MemberAccessExpressionNode &node) = 0;
		virtual T visit(const ToExpressionNode &node) = 0;
		virtual T visit(const FunctionArgument &node) = 0;
		virtual T visit(const LambdaExpressionNode &node) = 0;
		virtual T visit(const ComparisonExpressionNode &node) = 0;
		virtual T visit(const AssignmentExpressionNode &node) = 0;
		virtual T visit(const IndexExpressionNode &node) = 0;
		virtual T visit(const MatchBranch &node) = 0;
		virtual T visit(const MatchExpressionNode &node) = 0;
		virtual T visit(const TernaryExpressionNode &node) = 0;
		virtual T visit(const ParallelExpressionNode &node) = 0;
		virtual T visit(const StructField &node) = 0;
		virtual T visit(const StructExpressionNode &node) = 0;
		virtual T visit(const RangeExpressionNode &node) = 0;

		virtual T visit(const VariableDeclarationNode &node) = 0;
		virtual T visit(const FunctionDeclarationNode &node) = 0;
		virtual T visit(const FunctionDefinitionNode &node) = 0;
		virtual T visit(const ReturnStatementNode &node) = 0;
		virtual T visit(const YieldStatementNode &node) = 0;
		virtual T visit(const ImplStatementNode &node) = 0;
		virtual T visit(const ConstructorStatementNode &node) = 0;
		virtual T visit(const ConstructorStatementNode::ConstructorArgument &node) = 0;
		virtual T visit(const ClassDeclarationNode &node) = 0;
		virtual T visit(const UnionDeclarationNode &node) = 0;
		virtual T visit(const EnumDeclarationNode &node) = 0;
		virtual T visit(const TraitDeclarationNode &node) = 0;
		virtual T visit(const ModuleDeclarationNode &node) = 0;
		virtual T visit(const ImportStatementNode &node) = 0;

		virtual T visit(const IfStatementNode &node) = 0;
		virtual T visit(const ForStatementNode &node) = 0;
		virtual T visit(const WhileStatementNode &node) = 0;
		virtual T visit(const BreakStatementNode &node) = 0;
		virtual T visit(const ContinueStatementNode &node) = 0;

		virtual T visit(const TypeAliasNode &node) = 0;

		virtual T visit(const BlockNode &node) = 0;

		virtual T visit(const IdentifierTypeNode &node) = 0;
		virtual T visit(const GenericTypeNode &node) = 0;
		virtual T visit(const SumTypeNode &node) = 0;
		virtual T visit(const IntersectionTypeNode &node) = 0;
		virtual T visit(const PrefixedTypeNode &node) = 0;
	};
}

#endif // VISITOR_H
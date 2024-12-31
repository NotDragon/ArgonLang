
#ifndef ARGONLANG_ANALYSISVISITOR_H
#define ARGONLANG_ANALYSISVISITOR_H
#include "frontend/Visitor.h"
#include "Error/Result.h"

namespace ArgonLang {
	class AnalysisVisitor: Visitor<Result<bool>> {
		Result<bool> visit(const ASTNode& node);
		Result<bool> visit(const ExpressionNode& node);
		Result<bool> visit(const StatementNode& node);
		Result<bool> visit(const TypeNode& node);

		Result<bool> visit(const ProgramNode &node);

		Result<bool> visit(const IntegralLiteralNode &node);
		Result<bool> visit(const FloatLiteralNode &node);
		Result<bool> visit(const StringLiteralNode &node);
		Result<bool> visit(const CharLiteralNode &node);
		Result<bool> visit(const BooleanLiteralNode &node);
		Result<bool> visit(const IdentifierNode &node);

		Result<bool> visit(const BinaryExpressionNode &node);
		Result<bool> visit(const UnaryExpressionNode &node);

		Result<bool> visit(const FunctionCallExpressionNode &node);
		Result<bool> visit(const MemberAccessExpressionNode &node);
		Result<bool> visit(const ToExpressionNode &node);
		Result<bool> visit(const FunctionArgument &node);
		Result<bool> visit(const LambdaExpressionNode &node);
		Result<bool> visit(const ComparisonExpressionNode &node);
		Result<bool> visit(const AssignmentExpressionNode &node);
		Result<bool> visit(const IndexExpressionNode &node);
		Result<bool> visit(const MatchBranch &node);
		Result<bool> visit(const MatchExpressionNode &node);
		Result<bool> visit(const TernaryExpressionNode &node);
		Result<bool> visit(const ParallelExpressionNode &node);
		Result<bool> visit(const StructField &node);
		Result<bool> visit(const StructExpressionNode &node);
		Result<bool> visit(const RangeExpressionNode &node);

		Result<bool> visit(const VariableDeclarationNode &node);
		Result<bool> visit(const FunctionDeclarationNode &node);
		Result<bool> visit(const FunctionDefinitionNode &node);
		Result<bool> visit(const ReturnStatementNode &node);
		Result<bool> visit(const YieldStatementNode &node);
		Result<bool> visit(const ImplStatementNode &node);
		Result<bool> visit(const ConstructorStatementNode &node);
		Result<bool> visit(const ConstructorStatementNode::ConstructorArgument &node);
		Result<bool> visit(const ClassDeclarationNode &node);
		Result<bool> visit(const UnionDeclarationNode &node);
		Result<bool> visit(const IfStatementNode &node);
		Result<bool> visit(const ForStatementNode &node);
		Result<bool> visit(const WhileStatementNode &node);
		Result<bool> visit(const BreakStatementNode &node);
		Result<bool> visit(const ContinueStatementNode &node);

		Result<bool> visit(const TypeAliasNode &node);

		Result<bool> visit(const BlockNode &node);

		Result<bool> visit(const IdentifierTypeNode &node);
		Result<bool> visit(const GenericTypeNode &node);
		Result<bool> visit(const SumTypeNode &node);
		Result<bool> visit(const IntersectionTypeNode &node);
		Result<bool> visit(const PrefixedTypeNode &node);
	};
}

#endif

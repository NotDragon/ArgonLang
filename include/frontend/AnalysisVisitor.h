
#ifndef ARGONLANG_ANALYSISVISITOR_H
#define ARGONLANG_ANALYSISVISITOR_H
#include "frontend/Visitor.h"
#include "Error/Result.h"

namespace ArgonLang {
	class AnalysisVisitor: Visitor<Result<bool>> {
	public:
		Result<bool> visit(const ASTNode& node) override;
		Result<bool> visit(const ExpressionNode& node) override;
		Result<bool> visit(const StatementNode& node) override;
		Result<bool> visit(const TypeNode& node) override;

		Result<bool> visit(const ProgramNode &node) override;

		Result<bool> visit(const IntegralLiteralNode &node) override;
		Result<bool> visit(const FloatLiteralNode &node) override;
		Result<bool> visit(const StringLiteralNode &node) override;
		Result<bool> visit(const CharLiteralNode &node) override;
		Result<bool> visit(const BooleanLiteralNode &node) override;
		Result<bool> visit(const IdentifierNode &node) override;

		Result<bool> visit(const BinaryExpressionNode &node) override;
		Result<bool> visit(const UnaryExpressionNode &node) override;

		Result<bool> visit(const FunctionCallExpressionNode &node) override;
		Result<bool> visit(const MemberAccessExpressionNode &node) override;
		Result<bool> visit(const ToExpressionNode &node) override;
		Result<bool> visit(const FunctionArgument &node) override;
		Result<bool> visit(const LambdaExpressionNode &node) override;
		Result<bool> visit(const ComparisonExpressionNode &node) override;
		Result<bool> visit(const AssignmentExpressionNode &node) override;
		Result<bool> visit(const IndexExpressionNode &node) override;
		Result<bool> visit(const MatchBranch &node) override;
		Result<bool> visit(const MatchExpressionNode &node) override;
		Result<bool> visit(const TernaryExpressionNode &node) override;
		Result<bool> visit(const ParallelExpressionNode &node) override;
		Result<bool> visit(const StructField &node) override;
		Result<bool> visit(const StructExpressionNode &node) override;
		Result<bool> visit(const RangeExpressionNode &node) override;

		Result<bool> visit(const VariableDeclarationNode &node) override;
		Result<bool> visit(const FunctionDeclarationNode &node) override;
		Result<bool> visit(const FunctionDefinitionNode &node) override;
		Result<bool> visit(const ReturnStatementNode &node) override;
		Result<bool> visit(const YieldStatementNode &node) override;
		Result<bool> visit(const ImplStatementNode &node) override;
		Result<bool> visit(const ConstructorStatementNode &node) override;
		Result<bool> visit(const ConstructorStatementNode::ConstructorArgument &node) override;
		Result<bool> visit(const ClassDeclarationNode &node) override;
		Result<bool> visit(const UnionDeclarationNode &node) override;
		Result<bool> visit(const IfStatementNode &node) override;
		Result<bool> visit(const ForStatementNode &node) override;
		Result<bool> visit(const WhileStatementNode &node) override;
		Result<bool> visit(const BreakStatementNode &node) override;
		Result<bool> visit(const ContinueStatementNode &node) override;

		Result<bool> visit(const TypeAliasNode &node) override;

		Result<bool> visit(const BlockNode &node) override;

		Result<bool> visit(const IdentifierTypeNode &node) override;
		Result<bool> visit(const GenericTypeNode &node) override;
		Result<bool> visit(const SumTypeNode &node) override;
		Result<bool> visit(const IntersectionTypeNode &node) override;
		Result<bool> visit(const PrefixedTypeNode &node) override;

		Result<bool> visit(const EnumDeclarationNode &node) override;
		Result<bool> visit(const TraitDeclarationNode &node) override;
		Result<bool> visit(const ModuleDeclarationNode &node) override;
		Result<bool> visit(const ImportStatementNode &node) override;
	};
}

#endif

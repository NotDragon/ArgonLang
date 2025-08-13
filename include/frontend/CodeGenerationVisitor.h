
#ifndef ARGONLANG_CODEGENERATIONSVISITOR_H
#define ARGONLANG_CODEGENERATIONSVISITOR_H
#include "frontend/Visitor.h"
#include "Error/Result.h"
#include <set>

namespace ArgonLang {
	class ScopedStatementContext {
		bool& ref;
		bool old;
	public:
		ScopedStatementContext(bool& ref, bool val): ref(ref), old(ref) { ref = val; }
		~ScopedStatementContext() { ref = old; }
	};

	class CodeGenerationVisitor: Visitor<Result<std::string>> {
	public:
		bool isStatementContext = false;
	 	std::set<std::string> dependencies;
		Result<std::string> visit(const ASTNode& node) override;
		Result<std::string> visit(const ExpressionNode& node) override;
		Result<std::string> visit(const StatementNode& node) override;
		Result<std::string> visit(const TypeNode& node) override;

		Result<std::string> visit(const ProgramNode &node) override;

		Result<std::string> visit(const IntegralLiteralNode &node) override;
		Result<std::string> visit(const FloatLiteralNode &node) override;
		Result<std::string> visit(const StringLiteralNode &node) override;
		Result<std::string> visit(const CharLiteralNode &node) override;
		Result<std::string> visit(const BooleanLiteralNode &node) override;
		Result<std::string> visit(const IdentifierNode &node) override;

		Result<std::string> visit(const BinaryExpressionNode &node) override;
		Result<std::string> visit(const UnaryExpressionNode &node) override;

		Result<std::string> visit(const FunctionCallExpressionNode &node) override;
		Result<std::string> visit(const MemberAccessExpressionNode &node) override;
		Result<std::string> visit(const ToExpressionNode &node) override;
		Result<std::string> visit(const FunctionArgument &node) override;
		Result<std::string> visit(const LambdaExpressionNode &node) override;
		Result<std::string> visit(const ComparisonExpressionNode &node) override;
		Result<std::string> visit(const AssignmentExpressionNode &node) override;
		Result<std::string> visit(const IndexExpressionNode &node) override;
		Result<std::string> visit(const MatchBranch &node) override;
		Result<std::string> visit(const MatchExpressionNode &node) override;
		
		// Pattern node visitors
		Result<std::string> visit(const PatternNode &node) override;
		Result<std::string> visit(const WildcardPatternNode &node) override;
		Result<std::string> visit(const LiteralPatternNode &node) override;
		Result<std::string> visit(const IdentifierPatternNode &node) override;
		Result<std::string> visit(const ArrayPatternNode &node) override;
		Result<std::string> visit(const StructPatternNode &node) override;
		Result<std::string> visit(const ConstructorPatternNode &node) override;
		Result<std::string> visit(const TypePatternNode &node) override;
		Result<std::string> visit(const RangePatternNode &node) override;
		Result<std::string> visit(const TernaryExpressionNode &node) override;
		Result<std::string> visit(const ParallelExpressionNode &node) override;
		Result<std::string> visit(const StructField &node) override;
		Result<std::string> visit(const StructExpressionNode &node) override;
		Result<std::string> visit(const RangeExpressionNode &node) override;

		Result<std::string> visit(const VariableDeclarationNode &node) override;
		Result<std::string> visit(const FunctionDeclarationNode &node) override;
		Result<std::string> visit(const FunctionDefinitionNode &node) override;
		Result<std::string> visit(const ReturnStatementNode &node) override;
		Result<std::string> visit(const YieldStatementNode &node) override;
		Result<std::string> visit(const ImplStatementNode &node) override;
		Result<std::string> visit(const ConstructorStatementNode &node) override;
		Result<std::string> visit(const ConstructorStatementNode::ConstructorArgument &node) override;
		Result<std::string> visit(const ClassDeclarationNode &node) override;
		Result<std::string> visit(const UnionDeclarationNode &node) override;
		Result<std::string> visit(const EnumDeclarationNode &node) override;
		Result<std::string> visit(const TraitDeclarationNode &node) override;
		Result<std::string> visit(const ModuleDeclarationNode &node) override;
		Result<std::string> visit(const ImportStatementNode &node) override;
		Result<std::string> visit(const IfStatementNode &node) override;
		Result<std::string> visit(const ForStatementNode &node) override;
		Result<std::string> visit(const WhileStatementNode &node) override;
		Result<std::string> visit(const BreakStatementNode &node) override;
		Result<std::string> visit(const ContinueStatementNode &node) override;

		Result<std::string> visit(const TypeAliasNode &node) override;

		Result<std::string> visit(const BlockNode &node) override;

		Result<std::string> visit(const IdentifierTypeNode &node) override;
		Result<std::string> visit(const GenericTypeNode &node) override;
		Result<std::string> visit(const SumTypeNode &node) override;
		Result<std::string> visit(const IntersectionTypeNode &node) override;
		Result<std::string> visit(const PrefixedTypeNode &node) override;
		Result<std::string> visit(const FunctionTypeNode &node) override;
		Result<std::string> visit(const ArrayTypeNode &node) override;
		Result<std::string> visit(const VariadicTypeNode &node) override;
		
	private:
		// Helper method for generating destructuring assignments
		Result<std::string> generateDestructuring(const PatternNode* pattern, const std::string& sourceVar);
	};
}

#endif

//
// Created by User on 31/12/2024.
//
#include "frontend/AnalysisVisitor.h"
#include "backend/Parser.h"

using namespace ArgonLang;

Result<bool> AnalysisVisitor::visit(std::unique_ptr<ASTNode> node) {
	switch (node.get()->getNodeGroup()) {
		case ASTNodeGroup::Expression:
			return visit(dynamic_unique_cast<ExpressionNode>(std::move(node)));
		case ASTNodeGroup::Statement:
			return visit(dynamic_unique_cast<StatementNode>(std::move(node)));
		case ASTNodeGroup::Type:
			return visit(dynamic_unique_cast<TypeNode>(std::move(node)));
	}
}

Result<bool> AnalysisVisitor::visit(std::unique_ptr<ExpressionNode> node) {
	switch (node.get()->getNodeType()) {
		case ASTNodeType::IntegralLiteral:
			return visit(static_cast<const IntegralLiteralNode&>(*node));
		case ASTNodeType::FloatLiteral:
			return visit(static_cast<const FloatLiteralNode&>(*node));
		case ASTNodeType::StringLiteral:
			return visit(static_cast<const StringLiteralNode&>(*node));
		case ASTNodeType::CharLiteral:
			return visit(static_cast<const CharLiteralNode&>(*node));
		case ASTNodeType::BooleanLiteral:
			return visit(static_cast<const BooleanLiteralNode&>(*node));
		case ASTNodeType::Identifier:
			return visit(static_cast<const IdentifierNode&>(*node));
		case ASTNodeType::BinaryExpression:
			return visit(static_cast<const BinaryExpressionNode&>(*node));
		case ASTNodeType::UnaryExpression:
			return visit(static_cast<const UnaryExpressionNode&>(*node));
		case ASTNodeType::FunctionCallExpression:
			return visit(static_cast<const FunctionCallExpressionNode&>(*node));
		case ASTNodeType::MemberAccessExpression:
			return visit(static_cast<const MemberAccessExpressionNode&>(*node));
		case ASTNodeType::ToExpression:
			return visit(static_cast<const ToExpressionNode&>(*node));
		case ASTNodeType::LambdaExpression:
			return visit(static_cast<const LambdaExpressionNode&>(*node));
		case ASTNodeType::ComparisonExpression:
			return visit(static_cast<const ComparisonExpressionNode&>(*node));
		case ASTNodeType::AssignmentExpression:
			return visit(static_cast<const AssignmentExpressionNode&>(*node));
		case ASTNodeType::IndexExpression:
			return visit(static_cast<const IndexExpressionNode&>(*node));
		case ASTNodeType::MatchExpression:
			return visit(static_cast<const MatchExpressionNode&>(*node));
		case ASTNodeType::TernaryExpression:
			return visit(static_cast<const TernaryExpressionNode&>(*node));
		case ASTNodeType::ParallelExpression:
			return visit(static_cast<const ParallelExpressionNode&>(*node));
		case ASTNodeType::StructExpression:
			return visit(static_cast<const StructExpressionNode&>(*node));
		case ASTNodeType::RangeExpression:
			return visit(static_cast<const RangeExpressionNode&>(*node));
		default:
			return visit(std::move(node));
	}
}

Result<bool> AnalysisVisitor::visit(std::unique_ptr<StatementNode> node) {
	switch (node.get()->getNodeType()) {
		case ASTNodeType::ReturnStatement:
			return visit(static_cast<const ReturnStatementNode&>(*node));
		case ASTNodeType::VariableDeclaration:
			return visit(static_cast<const VariableDeclarationNode&>(*node));
		case ASTNodeType::IfStatement:
			return visit(static_cast<const IfStatementNode&>(*node));
		case ASTNodeType::ForStatement:
			return visit(static_cast<const ForStatementNode&>(*node));
		case ASTNodeType::UnionDeclaration:
			return visit(static_cast<const UnionDeclarationNode&>(*node));
		case ASTNodeType::YieldStatement:
			return visit(static_cast<const YieldStatementNode&>(*node));
		case ASTNodeType::WhileStatement:
			return visit(static_cast<const WhileStatementNode&>(*node));
		case ASTNodeType::BreakStatement:
			return visit(static_cast<const BreakStatementNode&>(*node));
		case ASTNodeType::ContinueStatement:
			return visit(static_cast<const ContinueStatementNode&>(*node));
		case ASTNodeType::Block:
			return visit(static_cast<const BlockNode&>(*node));
		case ASTNodeType::TypeAlias:
			return visit(static_cast<const TypeAliasNode&>(*node));
		case ASTNodeType::ClassDeclaration:
			return visit(static_cast<const ClassDeclarationNode&>(*node));
		case ASTNodeType::FunctionDeclaration:
			return visit(static_cast<const FunctionDeclarationNode&>(*node));
		case ASTNodeType::FunctionDefinition:
			return visit(static_cast<const FunctionDefinitionNode&>(*node));
		case ASTNodeType::ConstructorStatement:
			return visit(static_cast<const ConstructorStatementNode&>(*node));
		case ASTNodeType::ImplStatement:
			return visit(static_cast<const ImplStatementNode&>(*node));
		default:
			return visit(std::move(node));
	}
}

Result<bool> AnalysisVisitor::visit(std::unique_ptr<TypeNode> node) {

}


Result<bool> AnalysisVisitor::visit(const ProgramNode &node) {

}


Result<bool> AnalysisVisitor::visit(const IntegralLiteralNode &node) {

}

Result<bool> AnalysisVisitor::visit(const FloatLiteralNode &node) {

}

Result<bool> AnalysisVisitor::visit(const StringLiteralNode &node) {

}

Result<bool> AnalysisVisitor::visit(const CharLiteralNode &node) {

}

Result<bool> AnalysisVisitor::visit(const BooleanLiteralNode &node) {

}

Result<bool> AnalysisVisitor::visit(const IdentifierNode &node) {

}


Result<bool> AnalysisVisitor::visit(const BinaryExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const UnaryExpressionNode &node) {

}


Result<bool> AnalysisVisitor::visit(const FunctionCallExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const MemberAccessExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ToExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const FunctionArgument &node) {

}

Result<bool> AnalysisVisitor::visit(const LambdaExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ComparisonExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const AssignmentExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const IndexExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const MatchBranch &node) {

}

Result<bool> AnalysisVisitor::visit(const MatchExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const TernaryExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ParallelExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const StructField &node) {

}

Result<bool> AnalysisVisitor::visit(const StructExpressionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const RangeExpressionNode &node) {

}


Result<bool> AnalysisVisitor::visit(const VariableDeclarationNode &node) {

}

Result<bool> AnalysisVisitor::visit(const FunctionDeclarationNode &node) {

}

Result<bool> AnalysisVisitor::visit(const FunctionDefinitionNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ReturnStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const YieldStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ImplStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ConstructorStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ConstructorStatementNode::ConstructorArgument &node) {

}

Result<bool> AnalysisVisitor::visit(const ClassDeclarationNode &node) {

}

Result<bool> AnalysisVisitor::visit(const UnionDeclarationNode &node) {

}

Result<bool> AnalysisVisitor::visit(const IfStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ForStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const WhileStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const BreakStatementNode &node) {

}

Result<bool> AnalysisVisitor::visit(const ContinueStatementNode &node) {

}


Result<bool> AnalysisVisitor::visit(const TypeAliasNode &node) {

}


Result<bool> AnalysisVisitor::visit(const BlockNode &node) {

}


Result<bool> AnalysisVisitor::visit(const IdentifierTypeNode &node) {

}

Result<bool> AnalysisVisitor::visit(const GenericTypeNode &node) {

}

Result<bool> AnalysisVisitor::visit(const SumTypeNode &node) {

}

Result<bool> AnalysisVisitor::visit(const IntersectionTypeNode &node) {

}

Result<bool> AnalysisVisitor::visit(const PrefixedTypeNode &node) {

}
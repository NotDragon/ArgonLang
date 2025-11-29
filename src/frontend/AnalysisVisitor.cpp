//
// Created by User on 31/12/2024.
//
#include "frontend/AnalysisVisitor.h"

#include "backend/Parser.h"
#include "Error/Error.h"
#include "Error/Result.h"

#include <iostream>

using namespace ArgonLang;

Result<bool> AnalysisVisitor::visit(const ASTNode& node) {
	switch (node.get_node_group()) {
	case ASTNodeGroup::Expression:
		return visit(dynamic_cast<const ExpressionNode&>(node));
	case ASTNodeGroup::Statement:
		return visit(dynamic_cast<const StatementNode&>(node));
	case ASTNodeGroup::Type:
		return visit(dynamic_cast<const TypeNode&>(node));
	}
	return false; // Default case
}

Result<bool> AnalysisVisitor::visit(const ExpressionNode& node) {
	switch (node.get_node_type()) {
	case ASTNodeType::IntegralLiteral:
		return visit(dynamic_cast<const IntegralLiteralNode&>(node));
	case ASTNodeType::FloatLiteral:
		return visit(dynamic_cast<const FloatLiteralNode&>(node));
	case ASTNodeType::StringLiteral:
		return visit(dynamic_cast<const StringLiteralNode&>(node));
	case ASTNodeType::CharLiteral:
		return visit(dynamic_cast<const CharLiteralNode&>(node));
	case ASTNodeType::BooleanLiteral:
		return visit(dynamic_cast<const BooleanLiteralNode&>(node));
	case ASTNodeType::Identifier:
		return visit(dynamic_cast<const IdentifierNode&>(node));
	case ASTNodeType::BinaryExpression:
		return visit(dynamic_cast<const BinaryExpressionNode&>(node));
	case ASTNodeType::UnaryExpression:
		return visit(dynamic_cast<const UnaryExpressionNode&>(node));
	case ASTNodeType::FunctionCallExpression:
		return visit(dynamic_cast<const FunctionCallExpressionNode&>(node));
	case ASTNodeType::MemberAccessExpression:
		return visit(dynamic_cast<const MemberAccessExpressionNode&>(node));
	case ASTNodeType::ToExpression:
		return visit(dynamic_cast<const ToExpressionNode&>(node));
	case ASTNodeType::LambdaExpression:
		return visit(dynamic_cast<const LambdaExpressionNode&>(node));
	case ASTNodeType::ComparisonExpression:
		return visit(dynamic_cast<const ComparisonExpressionNode&>(node));
	case ASTNodeType::AssignmentExpression:
		return visit(dynamic_cast<const AssignmentExpressionNode&>(node));
	case ASTNodeType::IndexExpression:
		return visit(dynamic_cast<const IndexExpressionNode&>(node));
	case ASTNodeType::MatchExpression:
		return visit(dynamic_cast<const MatchExpressionNode&>(node));
	case ASTNodeType::TernaryExpression:
		return visit(dynamic_cast<const TernaryExpressionNode&>(node));
	case ASTNodeType::ParallelExpression:
		return visit(dynamic_cast<const ParallelExpressionNode&>(node));
	case ASTNodeType::StructExpression:
		return visit(dynamic_cast<const StructExpressionNode&>(node));
	case ASTNodeType::RangeExpression:
		return visit(dynamic_cast<const RangeExpressionNode&>(node));
	default:
		return Err<bool>(
		    create_parse_error(ErrorType::UnexpectedToken, "Unexpected Expression", Token::Position{0, 0}));
	}
}

Result<bool> AnalysisVisitor::visit(const StatementNode& node) {
	switch (node.get_node_type()) {
	case ASTNodeType::ReturnStatement:
		return visit(dynamic_cast<const ReturnStatementNode&>(node));
	case ASTNodeType::VariableDeclaration:
		return visit(dynamic_cast<const VariableDeclarationNode&>(node));
	case ASTNodeType::IfStatement:
		return visit(dynamic_cast<const IfStatementNode&>(node));
	case ASTNodeType::ForStatement:
		return visit(dynamic_cast<const ForStatementNode&>(node));
	case ASTNodeType::UnionDeclaration:
		return visit(dynamic_cast<const UnionDeclarationNode&>(node));
	case ASTNodeType::YieldStatement:
		return visit(dynamic_cast<const YieldStatementNode&>(node));
	case ASTNodeType::WhileStatement:
		return visit(dynamic_cast<const WhileStatementNode&>(node));
	case ASTNodeType::BreakStatement:
		return visit(dynamic_cast<const BreakStatementNode&>(node));
	case ASTNodeType::ContinueStatement:
		return visit(dynamic_cast<const ContinueStatementNode&>(node));
	case ASTNodeType::Block:
		return visit(dynamic_cast<const BlockNode&>(node));
	case ASTNodeType::TypeAlias:
		return visit(dynamic_cast<const TypeAliasNode&>(node));
	case ASTNodeType::ClassDeclaration:
		return visit(dynamic_cast<const ClassDeclarationNode&>(node));
	case ASTNodeType::FunctionDeclaration:
		return visit(dynamic_cast<const FunctionDeclarationNode&>(node));
	case ASTNodeType::FunctionDefinition:
		return visit(dynamic_cast<const FunctionDefinitionNode&>(node));
	case ASTNodeType::ConstructorStatement:
		return visit(dynamic_cast<const ConstructorStatementNode&>(node));
	case ASTNodeType::ImplStatement:
		return visit(dynamic_cast<const ImplStatementNode&>(node));
	case ASTNodeType::EnumDeclaration:
		return visit(dynamic_cast<const EnumDeclarationNode&>(node));
	case ASTNodeType::ConstraintDeclaration:
		return visit(dynamic_cast<const ConstraintDeclarationNode&>(node));
	case ASTNodeType::ModuleDeclaration:
		return visit(dynamic_cast<const ModuleDeclarationNode&>(node));
	case ASTNodeType::ImportStatement:
		return visit(dynamic_cast<const ImportStatementNode&>(node));
	default:
		return Err<bool>(create_parse_error(ErrorType::UnexpectedToken, "Unexpected Statement", Token::Position{0, 0}));
	}
}

Result<bool> AnalysisVisitor::visit(const TypeNode& node) {
	switch (node.get_node_type()) {
	case ASTNodeType::IntersectionType:
		return visit(dynamic_cast<const IntersectionTypeNode&>(node));
	case ASTNodeType::PrefixedType:
		return visit(dynamic_cast<const PrefixedTypeNode&>(node));
	case ASTNodeType::GenericType:
		return visit(dynamic_cast<const GenericTypeNode&>(node));
	case ASTNodeType::SumType:
		return visit(dynamic_cast<const SumTypeNode&>(node));
	case ASTNodeType::IdentifierType:
		return visit(dynamic_cast<const IdentifierTypeNode&>(node));
	default:
		return Err<bool>(create_parse_error(ErrorType::UnexpectedToken, "Unexpected Type", Token::Position{0, 0}));
	}
}

Result<bool> AnalysisVisitor::visit(const ProgramNode& node) {
	for (const auto& child : node.nodes) {
		auto result = visit(*child);
		if (!result.has_value()) {
			return Err<bool>(result.error());
		}
	}
	return {true};
}

Result<bool> AnalysisVisitor::visit(const IntegralLiteralNode& node) {
	std::cout << "IntegralLiteralNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const FloatLiteralNode& node) {
	std::cout << "FloatLiteralNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const StringLiteralNode& node) {
	std::cout << "StringLiteralNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const CharLiteralNode& node) {
	std::cout << "CharLiteralNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const BooleanLiteralNode& node) {
	std::cout << "BooleanLiteralNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const IdentifierNode& node) {
	std::cout << "IdentifierNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const BinaryExpressionNode& node) {
	std::cout << "BinaryExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const UnaryExpressionNode& node) {
	std::cout << "UnaryExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const FunctionCallExpressionNode& node) {
	std::cout << "FunctionCallExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const MemberAccessExpressionNode& node) {
	std::cout << "MemberAccessExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ToExpressionNode& node) {
	std::cout << "ToExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const FunctionArgument& node) {
	std::cout << "FunctionArgument\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const LambdaExpressionNode& node) {
	std::cout << "LambdaExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ComparisonExpressionNode& node) {
	std::cout << "ComparisonExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const AssignmentExpressionNode& node) {
	std::cout << "AssignmentExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const IndexExpressionNode& node) {
	std::cout << "IndexExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const MatchBranch& node) {
	std::cout << "MatchBranch\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const MatchExpressionNode& node) {
	std::cout << "MatchExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const TernaryExpressionNode& node) {
	std::cout << "TernaryExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ParallelExpressionNode& node) {
	std::cout << "ParallelExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const TryExpressionNode& node) {
	std::cout << "TryExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const StructField& node) {
	std::cout << "StructField\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const StructExpressionNode& node) {
	std::cout << "StructExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const RangeExpressionNode& node) {
	std::cout << "RangeExpressionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const VariableDeclarationNode& node) {
	std::cout << "VariableDeclarationNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const FunctionDeclarationNode& node) {
	std::cout << "FunctionDeclarationNode\n";
	visit(*node.body);
	return {};
}

Result<bool> AnalysisVisitor::visit(const FunctionDefinitionNode& node) {
	std::cout << "FunctionDefinitionNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ReturnStatementNode& node) {
	std::cout << "ReturnStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const YieldStatementNode& node) {
	std::cout << "YieldStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ImplStatementNode& node) {
	std::cout << "ImplStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ConstructorStatementNode& node) {
	std::cout << "ConstructorStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ConstructorStatementNode::ConstructorArgument& node) {
	std::cout << "ConstructorStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ClassDeclarationNode& node) {
	std::cout << "ClassDeclarationNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const UnionDeclarationNode& node) {
	std::cout << "UnionDeclarationNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const IfStatementNode& node) {
	std::cout << "IfStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ForStatementNode& node) {
	std::cout << "ForStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const WhileStatementNode& node) {
	std::cout << "WhileStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const BreakStatementNode& node) {
	std::cout << "BreakStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ContinueStatementNode& node) {
	std::cout << "ContinueStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const TypeAliasNode& node) {
	std::cout << "TypeAliasNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const BlockNode& node) {
	std::cout << "BlockNode\n";
	for (const auto& statement : node.body) {
		std::cout << "\t";
		visit(*statement);
	}
	return {};
}

Result<bool> AnalysisVisitor::visit(const IdentifierTypeNode& node) {
	std::cout << "IdentifierTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const GenericTypeNode& node) {
	std::cout << "GenericTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const SumTypeNode& node) {
	std::cout << "SumTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const IntersectionTypeNode& node) {
	std::cout << "IntersectionTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const PrefixedTypeNode& node) {
	std::cout << "PrefixedTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const EnumDeclarationNode& node) {
	std::cout << "EnumDeclarationNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ConstraintDeclarationNode& node) {
	std::cout << "ConstraintDeclarationNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ModuleDeclarationNode& node) {
	std::cout << "ModuleDeclarationNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ImportStatementNode& node) {
	std::cout << "ImportStatementNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const FunctionTypeNode& node) {
	std::cout << "FunctionTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ArrayTypeNode& node) {
	std::cout << "ArrayTypeNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const VariadicTypeNode& node) {
	std::cout << "VariadicTypeNode\n";
	return {};
}

// Pattern node implementations
Result<bool> AnalysisVisitor::visit(const PatternNode& node) {
	std::cout << "PatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const WildcardPatternNode& node) {
	std::cout << "WildcardPatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const LiteralPatternNode& node) {
	std::cout << "LiteralPatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const IdentifierPatternNode& node) {
	std::cout << "IdentifierPatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ArrayPatternNode& node) {
	std::cout << "ArrayPatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const StructPatternNode& node) {
	std::cout << "StructPatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const ConstructorPatternNode& node) {
	std::cout << "ConstructorPatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const TypePatternNode& node) {
	std::cout << "TypePatternNode\n";
	return {};
}

Result<bool> AnalysisVisitor::visit(const RangePatternNode& node) {
	std::cout << "RangePatternNode\n";
	return {};
}

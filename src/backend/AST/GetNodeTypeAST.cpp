#include "backend/AST.h"

using namespace ArgonLang;

ASTNodeType StringLiteralNode::getNodeType() const {
	return ASTNodeType::StringLiteral;
}

ASTNodeType CharLiteralNode::getNodeType() const {
	return ASTNodeType::CharLiteral;
}

ASTNodeType IntegralLiteralNode::getNodeType() const {
	return ASTNodeType::IntegralLiteral;
}

ASTNodeType FloatLiteralNode::getNodeType() const {
	return ASTNodeType::FloatLiteral;
}

ASTNodeType BooleanLiteralNode::getNodeType() const {
	return ASTNodeType::BooleanLiteral;
}

ASTNodeType IdentifierNode::getNodeType() const {
	return ASTNodeType::Identifier;
}

ASTNodeType BinaryExpressionNode::getNodeType() const {
	return ASTNodeType::BinaryExpression;
}

ASTNodeType UnaryExpressionNode::getNodeType() const {
	return ASTNodeType::UnaryExpression;
}

ASTNodeType NullExpressionNode::getNodeType() const {
	return ASTNodeType::NullExpression;
}

ASTNodeType FunctionCallExpressionNode::getNodeType() const {
	return ASTNodeType::FunctionCallExpression;
}

ASTNodeType ToExpressionNode::getNodeType() const {
	return ASTNodeType::ToExpression;
}

ASTNodeType LambdaExpressionNode::getNodeType() const {
	return ASTNodeType::LambdaExpression;
}

ASTNodeType ComparisonExpressionNode::getNodeType() const {
	return ASTNodeType::ComparisonExpression;
}

ASTNodeType AssignmentExpressionNode::getNodeType() const {
	return ASTNodeType::AssignmentExpression;
}

ASTNodeType IndexExpressionNode::getNodeType() const {
	return ASTNodeType::IndexExpression;
}

ASTNodeType MatchExpressionNode::getNodeType() const {
	return ASTNodeType::MatchExpression;
}

ASTNodeType TernaryExpressionNode::getNodeType() const {
	return ASTNodeType::TernaryExpression;
}

ASTNodeType ParallelExpressionNode::getNodeType() const {
	return ASTNodeType::ParallelExpression;
}

ASTNodeType StructExpressionNode::getNodeType() const {
	return ASTNodeType::StructExpression;
}

ASTNodeType RangeExpressionNode::getNodeType() const {
	return ASTNodeType::RangeExpression;
}

ASTNodeType ProgramNode::getNodeType() const {
	return ASTNodeType::Program;
}

ASTNodeType ReturnStatementNode::getNodeType() const {
	return ASTNodeType::ReturnStatement;
}

ASTNodeType VariableDeclarationNode::getNodeType() const {
	return ASTNodeType::VariableDeclaration;
}

ASTNodeType IfStatementNode::getNodeType() const {
	return ASTNodeType::IfStatement;
}

ASTNodeType ForStatementNode::getNodeType() const {
	return ASTNodeType::ForStatement;
}

ASTNodeType UnionDeclarationNode::getNodeType() const {
	return ASTNodeType::UnionDeclaration;
}

ASTNodeType YieldStatementNode::getNodeType() const {
	return ASTNodeType::YieldStatement;
}

ASTNodeType WhileStatementNode::getNodeType() const {
	return ASTNodeType::WhileStatement;
}

ASTNodeType BreakStatementNode::getNodeType() const {
	return ASTNodeType::BreakStatement;
}

ASTNodeType ContinueStatementNode::getNodeType() const {
	return ASTNodeType::ContinueStatement;
}

ASTNodeType BlockNode::getNodeType() const {
	return ASTNodeType::Block;
}

ASTNodeType SumTypeNode::getNodeType() const {
	return ASTNodeType::SumType;
}

ASTNodeType IdentifierTypeNode::getNodeType() const {
	return ASTNodeType::IdentifierType;
}

ASTNodeType TypeAliasNode::getNodeType() const {
	return ASTNodeType::TypeAlias;
}

ASTNodeType GenericTypeNode::getNodeType() const {
	return ASTNodeType::GenericType;
}

ASTNodeType StructField::getNodeType() const {
	return ASTNodeType::StructField;
}

ASTNodeType MatchBranch::getNodeType() const {
	return ASTNodeType::MatchBranch;
}

ASTNodeType FunctionArgument::getNodeType() const {
	return ASTNodeType::FunctionArgument;
}

ASTNodeType ClassDeclarationNode::getNodeType() const {
	return ASTNodeType::ClassDeclaration;
}

ASTNodeType FunctionDeclarationNode::getNodeType() const {
	return ASTNodeType::FunctionDeclaration;
}

ASTNodeType FunctionDefinitionNode::getNodeType() const {
	return ASTNodeType::FunctionDefinition;
}

ASTNodeType ConstructorStatementNode::getNodeType() const {
	return ASTNodeType::ConstructorStatement;
}

ASTNodeType ImplStatementNode::getNodeType() const {
	return ASTNodeType::ImplStatement;
}

ASTNodeType MemberAccessExpressionNode::getNodeType() const {
	return ASTNodeType::MemberAccessExpression;
}

ArgonLang::ASTNodeType ArgonLang::IntersectionTypeNode::getNodeType() const {
	return ArgonLang::ASTNodeType::IntersectionType;
}

ArgonLang::ASTNodeType ArgonLang::PrefixedTypeNode::getNodeType() const {
	return ArgonLang::ASTNodeType::PrefixedType;
}

// New AST node getNodeType implementations
ArgonLang::ASTNodeType ArgonLang::EnumDeclarationNode::getNodeType() const {
	return ArgonLang::ASTNodeType::EnumDeclaration;
}

ArgonLang::ASTNodeType ArgonLang::TraitDeclarationNode::getNodeType() const {
	return ArgonLang::ASTNodeType::TraitDeclaration;
}

ArgonLang::ASTNodeType ArgonLang::ModuleDeclarationNode::getNodeType() const {
	return ArgonLang::ASTNodeType::ModuleDeclaration;
}

ArgonLang::ASTNodeType ArgonLang::ImportStatementNode::getNodeType() const {
	return ArgonLang::ASTNodeType::ImportStatement;
}
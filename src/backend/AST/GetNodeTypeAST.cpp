#include "backend/AST.h"

using namespace ArgonLang;

ASTNodeType StringLiteralNode::get_node_type() const {
	return ASTNodeType::StringLiteral;
}

ASTNodeType CharLiteralNode::get_node_type() const {
	return ASTNodeType::CharLiteral;
}

ASTNodeType IntegralLiteralNode::get_node_type() const {
	return ASTNodeType::IntegralLiteral;
}

ASTNodeType FloatLiteralNode::get_node_type() const {
	return ASTNodeType::FloatLiteral;
}

ASTNodeType BooleanLiteralNode::get_node_type() const {
	return ASTNodeType::BooleanLiteral;
}

ASTNodeType IdentifierNode::get_node_type() const {
	return ASTNodeType::Identifier;
}

ASTNodeType BinaryExpressionNode::get_node_type() const {
	return ASTNodeType::BinaryExpression;
}

ASTNodeType UnaryExpressionNode::get_node_type() const {
	return ASTNodeType::UnaryExpression;
}

ASTNodeType NullExpressionNode::get_node_type() const {
	return ASTNodeType::NullExpression;
}

ASTNodeType FunctionCallExpressionNode::get_node_type() const {
	return ASTNodeType::FunctionCallExpression;
}

ASTNodeType ToExpressionNode::get_node_type() const {
	return ASTNodeType::ToExpression;
}

ASTNodeType LambdaExpressionNode::get_node_type() const {
	return ASTNodeType::LambdaExpression;
}

ASTNodeType ComparisonExpressionNode::get_node_type() const {
	return ASTNodeType::ComparisonExpression;
}

ASTNodeType AssignmentExpressionNode::get_node_type() const {
	return ASTNodeType::AssignmentExpression;
}

ASTNodeType IndexExpressionNode::get_node_type() const {
	return ASTNodeType::IndexExpression;
}

ASTNodeType SliceExpressionNode::get_node_type() const {
	return ASTNodeType::SliceExpression;
}

ASTNodeType MultipleIndexExpressionNode::get_node_type() const {
	return ASTNodeType::MultipleIndexExpression;
}

ASTNodeType MatchExpressionNode::get_node_type() const {
	return ASTNodeType::MatchExpression;
}

ASTNodeType TernaryExpressionNode::get_node_type() const {
	return ASTNodeType::TernaryExpression;
}

ASTNodeType ParallelExpressionNode::get_node_type() const {
	return ASTNodeType::ParallelExpression;
}

ASTNodeType TryExpressionNode::get_node_type() const {
	return ASTNodeType::TryExpression;
}

ASTNodeType StructExpressionNode::get_node_type() const {
	return ASTNodeType::StructExpression;
}

ASTNodeType RangeExpressionNode::get_node_type() const {
	return ASTNodeType::RangeExpression;
}

ASTNodeType ProgramNode::get_node_type() const {
	return ASTNodeType::Program;
}

ASTNodeType ReturnStatementNode::get_node_type() const {
	return ASTNodeType::ReturnStatement;
}

ASTNodeType VariableDeclarationNode::get_node_type() const {
	return ASTNodeType::VariableDeclaration;
}

ASTNodeType IfStatementNode::get_node_type() const {
	return ASTNodeType::IfStatement;
}

ASTNodeType ForStatementNode::get_node_type() const {
	return ASTNodeType::ForStatement;
}

ASTNodeType UnionDeclarationNode::get_node_type() const {
	return ASTNodeType::UnionDeclaration;
}

ASTNodeType YieldStatementNode::get_node_type() const {
	return ASTNodeType::YieldStatement;
}

ASTNodeType WhileStatementNode::get_node_type() const {
	return ASTNodeType::WhileStatement;
}

ASTNodeType BreakStatementNode::get_node_type() const {
	return ASTNodeType::BreakStatement;
}

ASTNodeType ContinueStatementNode::get_node_type() const {
	return ASTNodeType::ContinueStatement;
}

ASTNodeType BlockNode::get_node_type() const {
	return ASTNodeType::Block;
}

ASTNodeType SumTypeNode::get_node_type() const {
	return ASTNodeType::SumType;
}

ASTNodeType IdentifierTypeNode::get_node_type() const {
	return ASTNodeType::IdentifierType;
}

ASTNodeType TypeAliasNode::get_node_type() const {
	return ASTNodeType::TypeAlias;
}

ASTNodeType GenericTypeNode::get_node_type() const {
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

ASTNodeType ClassDeclarationNode::get_node_type() const {
	return ASTNodeType::ClassDeclaration;
}

ASTNodeType FunctionDeclarationNode::get_node_type() const {
	return ASTNodeType::FunctionDeclaration;
}

ASTNodeType FunctionDefinitionNode::get_node_type() const {
	return ASTNodeType::FunctionDefinition;
}

ASTNodeType ConstructorStatementNode::get_node_type() const {
	return ASTNodeType::ConstructorStatement;
}

ASTNodeType ImplStatementNode::get_node_type() const {
	return ASTNodeType::ImplStatement;
}

ASTNodeType MemberAccessExpressionNode::get_node_type() const {
	return ASTNodeType::MemberAccessExpression;
}

ArgonLang::ASTNodeType ArgonLang::IntersectionTypeNode::get_node_type() const {
	return ArgonLang::ASTNodeType::IntersectionType;
}

ArgonLang::ASTNodeType ArgonLang::PrefixedTypeNode::get_node_type() const {
	return ArgonLang::ASTNodeType::PrefixedType;
}

// New AST node getNodeType implementations
ArgonLang::ASTNodeType ArgonLang::EnumDeclarationNode::get_node_type() const {
	return ArgonLang::ASTNodeType::EnumDeclaration;
}

ArgonLang::ASTNodeType ArgonLang::ConstraintDeclarationNode::get_node_type() const {
	return ArgonLang::ASTNodeType::ConstraintDeclaration;
}

ArgonLang::ASTNodeType ArgonLang::GenericParameter::getNodeType() {
	return ArgonLang::ASTNodeType::GenericParameter;
}

ArgonLang::ASTNodeGroup ArgonLang::GenericParameter::getNodeGroup() {
	return ArgonLang::ASTNodeGroup::Type;
}

ArgonLang::ASTNodeType ArgonLang::ModuleDeclarationNode::get_node_type() const {
	return ArgonLang::ASTNodeType::ModuleDeclaration;
}

ArgonLang::ASTNodeType ArgonLang::ImportStatementNode::get_node_type() const {
	return ArgonLang::ASTNodeType::ImportStatement;
}

ArgonLang::ASTNodeType ArgonLang::FunctionTypeNode::get_node_type() const {
	return ArgonLang::ASTNodeType::FunctionType;
}

ArgonLang::ASTNodeType ArgonLang::ArrayTypeNode::get_node_type() const {
	return ArgonLang::ASTNodeType::ArrayType;
}

ArgonLang::ASTNodeType ArgonLang::VariadicTypeNode::get_node_type() const {
	return ArgonLang::ASTNodeType::VariadicType;
}

// Pattern node getNodeType implementations
ArgonLang::ASTNodeType ArgonLang::WildcardPatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::WildcardPattern;
}

ArgonLang::ASTNodeType ArgonLang::LiteralPatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::LiteralPattern;
}

ArgonLang::ASTNodeType ArgonLang::IdentifierPatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::IdentifierPattern;
}

ArgonLang::ASTNodeType ArgonLang::ArrayPatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::ArrayPattern;
}

ArgonLang::ASTNodeType ArgonLang::StructPatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::StructPattern;
}

ArgonLang::ASTNodeType ArgonLang::ConstructorPatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::ConstructorPattern;
}

ArgonLang::ASTNodeType ArgonLang::TypePatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::TypePattern;
}

ArgonLang::ASTNodeType ArgonLang::RangePatternNode::get_node_type() const {
	return ArgonLang::ASTNodeType::RangePattern;
}
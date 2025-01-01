#include "backend/AST.h"

std::string ArgonLang::primitiveTypeToString(PrimitiveType type) {
	switch (type) {
		case INT8: return "i8";
		case INT16: return "i16";
		case INT32: return "i32";
		case INT64: return "i64";
		case INT128: return "i128";
		case FLOAT32: return "f32";
		case FLOAT64: return "f64";
		case FLOAT128: return "f128";
	}
	return "";
}

ArgonLang::PrimitiveType ArgonLang::determineIntegerType(const std::string& value) {
	if (value.ends_with("i8")) return PrimitiveType::INT8;
	if (value.ends_with("i16")) return PrimitiveType::INT16;
	if (value.ends_with("i32")) return PrimitiveType::INT32;
	if (value.ends_with("i64")) return PrimitiveType::INT64;
	if (value.ends_with("i128")) return PrimitiveType::INT128;

	return PrimitiveType::INT32;
}

ArgonLang::PrimitiveType ArgonLang::determineFloatType(const std::string& value) {
	if (value.ends_with("f32")) return PrimitiveType::FLOAT32;
	if (value.ends_with("f64")) return PrimitiveType::FLOAT64;
	if (value.ends_with("f128")) return PrimitiveType::FLOAT128;

	return PrimitiveType::FLOAT32;
}


std::string ArgonLang::ASTNodeTypeToString(ASTNodeType type) {
	switch (type) {
		case ASTNodeType::StringLiteral: return "StringLiteral";
		case ASTNodeType::IntegralLiteral: return "IntegralLiteral";
		case ASTNodeType::FloatLiteral: return "FloatLiteral";
		case ASTNodeType::BooleanLiteral: return "BooleanLiteral";
		case ASTNodeType::Identifier: return "Identifier";
		case ASTNodeType::BinaryExpression: return "BinaryExpression";
		case ASTNodeType::UnaryExpression: return "UnaryExpression";
		case ASTNodeType::NullExpression: return "NullExpression";
		case ASTNodeType::FunctionCallExpression: return "FunctionCallExpression";
		case ASTNodeType::ToExpression: return "ToExpression";
		case ASTNodeType::LambdaExpression: return "LambdaExpression";
		case ASTNodeType::ComparisonExpression: return "ComparisonExpression";
		case ASTNodeType::AssignmentExpression: return "AssignmentExpression";
		case ASTNodeType::IndexExpression: return "IndexExpression";
		case ASTNodeType::MatchExpression: return "MatchExpression";
		case ASTNodeType::TernaryExpression: return "TernaryExpression";
		case ASTNodeType::ParallelExpression: return "ParallelExpression";
		case ASTNodeType::StructExpression: return "StructExpression";
		case ASTNodeType::RangeExpression: return "RangeExpression";
		case ASTNodeType::Program: return "Program";
		case ASTNodeType::ReturnStatement: return "ReturnStatement";
		case ASTNodeType::VariableDeclaration: return "VariableDeclaration";
		case ASTNodeType::IfStatement: return "IfStatement";
		case ASTNodeType::ForStatement: return "ForStatement";
		case ASTNodeType::UnionDeclaration: return "UnionDeclaration";
		case ASTNodeType::YieldStatement: return "YieldStatement";
		case ASTNodeType::WhileStatement: return "WhileStatement";
		case ASTNodeType::BreakStatement: return "BreakStatement";
		case ASTNodeType::ContinueStatement: return "ContinueStatement";
		case ASTNodeType::Block: return "Block";
		case ASTNodeType::SumType: return "SumType";
		case ASTNodeType::IdentifierType: return "IdentifierType";
		case ASTNodeType::TypeAlias: return "TypeAlias";
		case ASTNodeType::GenericType: return "GenericType";
		case ASTNodeType::StructField: return "StructField";
		case ASTNodeType::MatchBranch: return "MatchBranch";
		case ASTNodeType::FunctionArgument: return "FunctionArgument";
		case ASTNodeType::ClassDeclaration: return "ClassDeclaration";
		case ASTNodeType::FunctionDeclaration: return "FunctionDeclaration";
		case ASTNodeType::FunctionDefinition: return "FunctionDefinition";
		case ASTNodeType::ConstructorStatement: return "ConstructorStatement";
		case ASTNodeType::ImplStatement: return "ImplStatement";
		case ASTNodeType::CharLiteral: return "CharLiteral";
		case ASTNodeType::MemberAccessExpression: return "MemberAccessExpression";
		case ASTNodeType::IntersectionType: return "IntersectionType";
		case ASTNodeType::PrefixedType: return "PrefixedType";
	}
}

ArgonLang::ASTNodeGroup ArgonLang::ExpressionNode::getNodeGroup() const {
	return ArgonLang::ASTNodeGroup::Expression;
}

ArgonLang::ASTNodeGroup ArgonLang::StatementNode::getNodeGroup() const {
	return ArgonLang::ASTNodeGroup::Statement;
}

ArgonLang::ASTNodeGroup ArgonLang::TypeNode::getNodeGroup() const {
	return ArgonLang::ASTNodeGroup::Type;
}


// Created by User on 31/12/2024.
//
#include "frontend/CodeGenerationVisitor.h"
#include "backend/Parser.h"
#include <iostream>

using namespace ArgonLang;

Result<std::string> CodeGenerationVisitor::visit(const ASTNode& node) {
	switch (node.getNodeGroup()) {
		case ASTNodeGroup::Expression:
			return visit(dynamic_cast<const ExpressionNode&>(node));
		case ASTNodeGroup::Statement:
			return visit(dynamic_cast<const StatementNode&>(node));
		case ASTNodeGroup::Type:
			return visit(dynamic_cast<const TypeAliasNode&>(node));
	}
}

Result<std::string> CodeGenerationVisitor::visit(const ExpressionNode& node) {
	switch (node.getNodeType()) {
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
			return { "Unexpected Expression", "", Trace() };
	}
}

Result<std::string> CodeGenerationVisitor::visit(const StatementNode& node) {
	switch (node.getNodeType()) {
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
		default:
			return { "Unexpected Statement", "", Trace() };
	}
}

Result<std::string> CodeGenerationVisitor::visit(const TypeNode& node) {
	switch (node.getNodeType()) {
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
			return { "Unexpected Type", "", Trace() };
	}
}


Result<std::string> CodeGenerationVisitor::visit(const ProgramNode &node) {
	std::string code;
	for (const auto& child : node.nodes) {
		auto result = visit(*child);
		if (result.hasError()) return result;
		code += result.getValue();
	}
	return { code };
}


Result<std::string> CodeGenerationVisitor::visit(const IntegralLiteralNode &node) {
	switch (node.type) {
		case PrimitiveType::INT8:   return std::to_string(node.value.i8);
		case PrimitiveType::INT16:  return std::to_string(node.value.i16);
		case PrimitiveType::INT32:  return std::to_string(node.value.i32);
		case PrimitiveType::INT64:  return std::to_string(node.value.i64);
		case PrimitiveType::INT128: return std::to_string(static_cast<int64_t>(node.value.i128));
		default:
			return { "Invalid Integer", "", Trace(node.getNodeType(), node.position) };
	}
}

Result<std::string> CodeGenerationVisitor::visit(const FloatLiteralNode &node) {
	switch (node.type) {
		case PrimitiveType::FLOAT32:  return std::to_string(node.value.f32);
		case PrimitiveType::FLOAT64:  return std::to_string(node.value.f64);
		case PrimitiveType::FLOAT128: return std::to_string(node.value.f128);
		default:
			return { "Invalid Float", "", Trace(node.getNodeType(), node.position) };
	}

}

Result<std::string> CodeGenerationVisitor::visit(const StringLiteralNode &node) {
	return { node.value };
}

Result<std::string> CodeGenerationVisitor::visit(const CharLiteralNode &node) {
	return { std::to_string(node.value) };
}

Result<std::string> CodeGenerationVisitor::visit(const BooleanLiteralNode &node) {
	return { node.value ? "true" : "false" };
}

Result<std::string> CodeGenerationVisitor::visit(const IdentifierNode &node) {
	return { node.identifier };
}


Result<std::string> CodeGenerationVisitor::visit(const BinaryExpressionNode &node) {
	Result<std::string> left = visit(*node.left);
	if(left.hasError()) return left;

	Result<std::string> right = visit(*node.right);
	if(left.hasError()) return right;

	return { left.getValue() + " " + node.op.value + " " + right.getValue() };
}

Result<std::string> CodeGenerationVisitor::visit(const UnaryExpressionNode &node) {
	Result<std::string> operand = visit(*node.operand);
	if(operand.hasError()) return operand;

	return { node.op.value + " " + operand.getValue() };
}


Result<std::string> CodeGenerationVisitor::visit(const FunctionCallExpressionNode &node) {
	Result<std::string> functionName = visit(*node.function);
	if(functionName.hasError()) return functionName;

	std::string code = functionName.getValue() + "(";

	for (const auto &arg: node.arguments) {
		Result<std::string> argResult = visit(*arg);
		if (argResult.hasError()) return argResult;
		code += argResult.getValue() + ", ";
	}
	code += ")";
	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const MemberAccessExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ToExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionArgument &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const LambdaExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ComparisonExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const AssignmentExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const IndexExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const MatchBranch &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const MatchExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const TernaryExpressionNode &node) {
	return { "" };
	
}

Result<std::string> CodeGenerationVisitor::visit(const ParallelExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const StructField &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const StructExpressionNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const RangeExpressionNode &node) {
	return { "" };
}


Result<std::string> CodeGenerationVisitor::visit(const VariableDeclarationNode &node) {
	std::string code;
	if(node.type) {
		Result<std::string> type = visit(*node.type);
		if(type.hasError()) return type;
		code += type.getValue() + " ";
	} else code += "auto ";

	code += node.name;

	if (node.value) {
		Result<std::string> value = visit(*node.value);
		if (value.hasError()) return value;
		code += " = " + value.getValue();
	}

	code += ";";

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionDeclarationNode &node) {
	Result<std::string> functionName = visit(*node.name);
	if(functionName.hasError()) return functionName;
	std::string code = "int " + functionName.getValue() + "(";
	for (const auto &arg: node.args) {
		Result<std::string> type = visit(*arg->type);
		if(type.hasError()) return type;

		code += type.getValue() + " " + arg->name + ",";
	}
	if (!node.args.empty()) {
		code.pop_back();
	}

	code += ")";
	if(node.body->getNodeType() != ASTNodeType::Block) code += "{";

	Result<std::string> body = visit(*node.body);
	if(body.hasError()) return body;
	code += body.getValue();
	if(node.body->getNodeType() != ASTNodeType::Block) code += "}";

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionDefinitionNode &node) {
	Result<std::string> functionName = visit(*node.name);
	if(functionName.hasError()) return functionName;
	std::string returnType = "auto";


	std::string code = "int " + functionName.getValue() + "(";
	for (const auto &arg: node.args) {
		Result<std::string> type = visit(*arg->type);
		if(type.hasError()) return type;

		code += type.getValue() + " " + arg->name + ",";
	}
	if (!node.args.empty()) {
		code.pop_back();
	}

	code += ");";
	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const ReturnStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const YieldStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ImplStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorStatementNode::ConstructorArgument &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ClassDeclarationNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const UnionDeclarationNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const IfStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ForStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const WhileStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const BreakStatementNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const ContinueStatementNode &node) {
	return { "" };
}


Result<std::string> CodeGenerationVisitor::visit(const TypeAliasNode &node) {
	return { "" };
}


Result<std::string> CodeGenerationVisitor::visit(const BlockNode &node) {
	std::string code = "{";

	for(const auto& statement: node.body) {
		Result<std::string> statementCode = visit(*statement);
		if(statementCode.hasError()) return statementCode;
		code += statementCode.getValue();
	}

	code += "}";
	return code;
}


Result<std::string> CodeGenerationVisitor::visit(const IdentifierTypeNode &node) {
	std::string code;
	if(node.typeName == "i8")
		code += "int8_t";
	else if(node.typeName == "i16")
		code += "int16_t";
	else if(node.typeName == "i32")
		code += "int32_t";
	else if(node.typeName == "i64")
		code += "int64_t";
	else if(node.typeName == "i128")
		code += "__int128";
	else if(node.typeName == "f32")
		code += "float";
	else if(node.typeName == "f64")
		code += "double";
	else if(node.typeName == "f128")
		code += "long double";
	else
		code += node.typeName;

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const GenericTypeNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const SumTypeNode &node) {
	std::string code = "std::variant<";
	for(const auto& type: node.types) {
		Result<std::string> typeCode = visit(*type);
		if(typeCode.hasError()) return typeCode;

		code += typeCode.getValue() + ",";
	}

	if (!node.types.empty()) {
		code.pop_back();
	}
	code += ">";

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const IntersectionTypeNode &node) {
	return { "" };
}

Result<std::string> CodeGenerationVisitor::visit(const PrefixedTypeNode &node) {
	return { "" };
}
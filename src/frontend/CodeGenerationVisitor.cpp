
// Created by User on 31/12/2024.
//
#include "frontend/CodeGenerationVisitor.h"
#include "backend/Parser.h"
#include "Error/ErrorFormatter.h"
#include <iostream>

using namespace ArgonLang;

Result<std::string> CodeGenerationVisitor::visit(const ASTNode& node) {
	switch (node.getNodeGroup()) {
		case ASTNodeGroup::Expression:
			return visit(dynamic_cast<const ExpressionNode&>(node));
		case ASTNodeGroup::Statement:
			return visit(dynamic_cast<const StatementNode&>(node));
		case ASTNodeGroup::Type:
			return visit(dynamic_cast<const TypeNode&>(node));
	}
	return { 
		ErrorFormatter::formatCodeGenError("Unknown node group", "ASTNode"),
		"",
		Trace(ASTNodeType::Program, node.position)
	};
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
			return { 
				ErrorFormatter::formatCodeGenError("Unexpected expression type", ASTNodeTypeToString(node.getNodeType())),
				"",
				Trace(node.getNodeType(), node.position)
			};
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
		case ASTNodeType::EnumDeclaration:
			return visit(dynamic_cast<const EnumDeclarationNode&>(node));
		case ASTNodeType::TraitDeclaration:
			return visit(dynamic_cast<const TraitDeclarationNode&>(node));
		case ASTNodeType::ModuleDeclaration:
			return visit(dynamic_cast<const ModuleDeclarationNode&>(node));
		case ASTNodeType::ImportStatement:
			return visit(dynamic_cast<const ImportStatementNode&>(node));
		default:
			return { 
				ErrorFormatter::formatCodeGenError("Unexpected statement type", ASTNodeTypeToString(node.getNodeType())),
				"",
				Trace(node.getNodeType(), node.position)
			};
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
		case ASTNodeType::FunctionType:
			return visit(dynamic_cast<const FunctionTypeNode&>(node));
		case ASTNodeType::ArrayType:
			return visit(dynamic_cast<const ArrayTypeNode&>(node));
		case ASTNodeType::VariadicType:
			return visit(dynamic_cast<const VariadicTypeNode&>(node));
		default:
			return { 
				ErrorFormatter::formatCodeGenError("Unexpected type", ASTNodeTypeToString(node.getNodeType())),
				"",
				Trace(node.getNodeType(), node.position)
			};
	}
}


Result<std::string> CodeGenerationVisitor::visit(const ProgramNode &node) {
	std::string code = "#include <cstdint>\n";
	code += "#include <algorithm>\n";
	code += "#include <numeric>\n";
	code += "#include <ranges>\n";
	code += "#include <memory>\n";
	code += "#include <functional>\n";
	code += "#include <utility>\n";
	code += "#include <iterator>\n";
	code += "#include <variant>\n";
	code += "#include <vector>\n";
	code += "#include <iostream>\n";
	code += "#include <future>\n";
	code += "#include <thread>\n";
	code += "#include <chrono>\n";
	code += "\n";
	
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
			return { 
				ErrorFormatter::formatCodeGenError("Invalid integer type", primitiveTypeToString(node.type)),
				ErrorFormatter::createContext("Integer literal code generation"),
				Trace(node.getNodeType(), node.position)
			};
	}
}

Result<std::string> CodeGenerationVisitor::visit(const FloatLiteralNode &node) {
	switch (node.type) {
		case PrimitiveType::FLOAT32:  return std::to_string(node.value.f32);
		case PrimitiveType::FLOAT64:  return std::to_string(node.value.f64);
		case PrimitiveType::FLOAT128: return std::to_string(node.value.f128);
		default:
			return { 
				ErrorFormatter::formatCodeGenError("Invalid float type", primitiveTypeToString(node.type)),
				ErrorFormatter::createContext("Float literal code generation"),
				Trace(node.getNodeType(), node.position)
			};
	}

}

Result<std::string> CodeGenerationVisitor::visit(const StringLiteralNode &node) {
	return { "\"" + node.value + "\"" };
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
	if(right.hasError()) return right;

	// Handle functional programming operators
	if(node.op.value == "|>") {
		// Pipe operator: left |> right becomes right(left)
		return { right.getValue() + "(" + left.getValue() + ")" };
	}
	else if(node.op.value == "||>") {
		// Map pipe: left ||> right becomes std::transform with right applied to each element
		return { "std::transform(" + left.getValue() + ".begin(), " + left.getValue() + ".end(), " + left.getValue() + ".begin(), " + right.getValue() + ")" };
	}
	else if(node.op.value == "|") {
		// Filter operator: left | right becomes std::copy_if with right as predicate
		return { "[&]() { auto result = decltype(" + left.getValue() + "){}; std::copy_if(" + left.getValue() + ".begin(), " + left.getValue() + ".end(), std::back_inserter(result), " + right.getValue() + "); return result; }()" };
	}
	else if(node.op.value == "&") {
		// Map operator: left & right becomes std::transform with right applied to each element
		return { "[&]() { auto result = decltype(" + left.getValue() + "){}; std::transform(" + left.getValue() + ".begin(), " + left.getValue() + ".end(), std::back_inserter(result), " + right.getValue() + "); return result; }()" };
	}
	else if(node.op.value == "^") {
		// Reduce operator: left ^ right becomes std::accumulate with binary operation
		return { "std::accumulate(" + left.getValue() + ".begin(), " + left.getValue() + ".end(), typename decltype(" + left.getValue() + ")::value_type{}, " + right.getValue() + ")" };
	}
	else if(node.op.value == "^^") {
		// Accumulate range: left ^^ right becomes std::accumulate with custom logic
		return { "std::accumulate(" + left.getValue() + ".begin(), " + left.getValue() + ".end(), decltype(" + left.getValue() + "){}, " + right.getValue() + ")" };
	}
	else if(node.op.value == "to") {
		// Range operator: handled by ToExpressionNode, but if it appears here as binary op
		return { "std::ranges::iota_view(" + left.getValue() + ", " + right.getValue() + ")" };
	}
	else {
		// Regular binary operators
		return { left.getValue() + " " + node.op.value + " " + right.getValue() };
	}
}

Result<std::string> CodeGenerationVisitor::visit(const UnaryExpressionNode &node) {
	Result<std::string> operand = visit(*node.operand);
	if(operand.hasError()) return operand;

	// Handle special unary operators
	if(node.op.value == "$") {
		// Iterator syntax: $arr becomes {arr.begin(), arr.end()}
		return { "std::make_pair(" + operand.getValue() + ".begin(), " + operand.getValue() + ".end())" };
	}
	else if(node.op.value == "~") {
		// Ownership operator: ~ptr becomes std::unique_ptr or move semantics
		return { "std::make_unique<decltype(" + operand.getValue() + ")>(" + operand.getValue() + ")" };
	}
	else if(node.op.value == "&") {
		// Reference operator (immutable reference)
		return { "&" + operand.getValue() };
	}
	else if(node.op.value == "&&") {
		// Mutable reference operator  
		return { "&" + operand.getValue() };
	}
	else {
		// Regular unary operators
		return { node.op.value + " " + operand.getValue() };
	}
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
	code.erase(code.size() - 2);
	code += ")";

	if(this->isStatementContext) code += ";";
	
	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const MemberAccessExpressionNode &node) {
	Result<std::string> parent = visit(*node.parent);
	if(parent.hasError()) return parent;
	Result<std::string> member = visit(*node.member);
	if (member.hasError()) return member;

	return { parent.getValue() + node.accessType.value + member.getValue() };
}

Result<std::string> CodeGenerationVisitor::visit(const ToExpressionNode &node) {
	Result<std::string> lowerBound = visit(*node.lowerBound);
	if(lowerBound.hasError()) return lowerBound;

	Result<std::string> upperBound = visit(*node.upperBound);
	if(upperBound.hasError()) return upperBound;

	return {
		"std::ranges::iota_view(" + lowerBound.getValue() + "," + upperBound.getValue() + (node.isInclusive? "+1": "") + ")"
	};
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionArgument &node) {
	Result<std::string> type = visit(*node.type);
	if(type.hasError()) return type;


	return { type.getValue() + " " + node.name };
}

Result<std::string> CodeGenerationVisitor::visit(const LambdaExpressionNode &node) {
	std::string code = "[&](";

	for (const auto& param : node.parameters) {
		auto paramResult = visit(*param);
		if (paramResult.hasError()) return paramResult;
		code += paramResult.getValue() + ",";
	}

	if (!node.parameters.empty()) code.pop_back();
	code += ")";

	Result<std::string> body = visit(*node.body);
	if(body.hasError()) return body;

	if(node.body->getNodeType() != ASTNodeType::Block)
		code += "{" + body.getValue() + "}";
	else
		code += body.getValue();

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ComparisonExpressionNode &node) {
	Result<std::string> left = visit(*node.left);
	if(left.hasError()) return left;

	Result<std::string> right = visit(*node.right);
	if(right.hasError()) return right;

	return { left.getValue() + node.op.value + right.getValue() };
}

Result<std::string> CodeGenerationVisitor::visit(const AssignmentExpressionNode &node) {
	Result<std::string> left = visit(*node.left);
	if(left.hasError()) return left;

	Result<std::string> right = visit(*node.right);
	if(right.hasError()) return right;

	return { left.getValue() + node.op.value + right.getValue() };
}

Result<std::string> CodeGenerationVisitor::visit(const IndexExpressionNode &node) {
	Result<std::string> array = visit(*node.array);
	if(array.hasError()) return array;

	Result<std::string> index = visit(*node.index);
	if(index.hasError()) return index;

	return { array.getValue() + "[" + index.getValue() + "]" };
}

Result<std::string> CodeGenerationVisitor::visit(const MatchBranch &node) {
    std::string pattern;
    if(node.pattern) {
    	auto p = visit(*node.pattern);
        if(p.hasError()) return p;
        pattern = p.getValue();
    }

    auto body = visit(*node.body);
    if(body.hasError()) return body;

	std::string code;
	if (pattern == "_") {
		code = "if (true) {";
	} else if (pattern.find("to") != std::string::npos && pattern.find("=") != std::string::npos) {
		// Handle range patterns like "1 to= 10"
		size_t toPos = pattern.find(" to");
		if (toPos != std::string::npos) {
			std::string start = pattern.substr(0, toPos);
			std::string end = pattern.substr(toPos + 4); // Skip " to="
			code = "if(__match_val >= " + start + " && __match_val <= " + end + ") {";
		} else {
			code = "if(__match_val == " + pattern + ") {";
		}
	} else if (pattern.find("to") != std::string::npos) {
		// Handle exclusive range patterns like "1 to 10"
		size_t toPos = pattern.find(" to ");
		if (toPos != std::string::npos) {
			std::string start = pattern.substr(0, toPos);
			std::string end = pattern.substr(toPos + 4);
			code = "if(__match_val >= " + start + " && __match_val < " + end + ") {";
		} else {
			code = "if(__match_val == " + pattern + ") {";
		}
	} else if (pattern.find("&&") != std::string::npos) {
		// Handle guard conditions like "x && x > 5"
		size_t guardPos = pattern.find(" && ");
		if (guardPos != std::string::npos) {
			std::string var = pattern.substr(0, guardPos);
			std::string condition = pattern.substr(guardPos + 4);
			code = "if(auto " + var + " = __match_val; " + condition + ") {";
		} else {
			code = "if(__match_val == " + pattern + ") {";
		}
	} else {
		code = "if(__match_val == " + pattern + ") {";
	}

	if((*node.body).getNodeGroup() == ASTNodeGroup::Expression) {
		code += "return " + body.getValue() + ";}";
	} else {
		code += body.getValue() + "}";
	}

    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const MatchExpressionNode &node) {
		ScopedStatementContext scoped(this->isStatementContext, false);
        auto value = visit(*node.value);
        if(value.hasError()) return value;

        std::string code = "([&]() { auto __match_val = " + value.getValue() + ";";
        bool first = true;
        for(const auto &branch : node.branches) {
            auto branchCode = visit(*branch);
            if(branchCode.hasError()) return branchCode;
            if(!first) code += " else ";
            code += branchCode.getValue();
            first = false;
        }
        code += "})()";
        return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const TernaryExpressionNode &node) {
        auto cond = visit(*node.condition);
        if(cond.hasError()) return cond;
        auto t = visit(*node.trueBranch);
        if(t.hasError()) return t;
        auto f = visit(*node.falseBranch);
        if(f.hasError()) return f;

        return { cond.getValue() + " ? " + t.getValue() + " : " + f.getValue() };

}

Result<std::string> CodeGenerationVisitor::visit(const ParallelExpressionNode &node) {
        auto stmt = visit(*node.statementNode);
        if(stmt.hasError()) return stmt;
        return { "std::async(std::launch::async, [&]() { " + stmt.getValue() + "; return 0; })" };
}

Result<std::string> CodeGenerationVisitor::visit(const StructField &node) {
        std::string code = "." + node.name;
        if(node.value) {
                auto val = visit(*node.value);
                if(val.hasError()) return val;
                code += " = " + val.getValue();
        }
        return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const StructExpressionNode &node) {
        std::string code = "{";
        for(const auto &field : node.fields) {
                auto fieldCode = visit(field);
                if(fieldCode.hasError()) return fieldCode;
                code += fieldCode.getValue() + ",";
        }
        if(!node.fields.empty()) code.pop_back();
        code += "}";
        return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const RangeExpressionNode &node) {
	std::string code = "{";

	for(const auto& i: node.range) {
		Result<std::string> item = visit(*i);
		if(item.hasError()) return item;
		code += item.getValue() + ",";
	}

	code.pop_back();
	code += "}";
	return { code };
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
	ScopedStatementContext scoped(this->isStatementContext, true);

	auto nameResult = visit(*node.name);
	if (nameResult.hasError()) return nameResult;
	std::string functionName = nameResult.getValue();

	std::string returnType = functionName == "main"? "int": "auto";
	if (node.returnType) {
		auto returnTypeResult = visit(*node.returnType);
		if (returnTypeResult.hasError()) return returnTypeResult;
		returnType = returnTypeResult.getValue();
	}

	std::string code = returnType + " " + functionName + "(";

	for (const auto& arg : node.args) {
		auto argResult = visit(*arg);
		if (argResult.hasError()) return argResult;
		code += argResult.getValue() + ",";
	}
	if (!node.args.empty()) {
		code.pop_back();
	}

	code += ")";

	if (node.body->getNodeType() != ASTNodeType::Block) {
		auto bodyResult = visit(*node.body);
		if (bodyResult.hasError()) return bodyResult;
		code += " { return " + bodyResult.getValue() + "; }";
	} else {
		auto bodyResult = visit(*node.body);
		if (bodyResult.hasError()) return bodyResult;
		code += bodyResult.getValue();
	}

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
	Result<std::string> returnValue = visit(*node.returnExpression);
	if(returnValue.hasError()) return returnValue;

	return { "return " + returnValue.getValue() + ";" };
}

Result<std::string> CodeGenerationVisitor::visit(const YieldStatementNode &node) {
	Result<std::string> yieldValue = visit(*node.expressionNode);
	if(yieldValue.hasError()) return yieldValue;

	return { "co_yield " + yieldValue.getValue() };
}

Result<std::string> CodeGenerationVisitor::visit(const ImplStatementNode &node) {
    std::string code = "// impl " + node.className;
    auto body = visit(*node.body);
    if(body.hasError()) return body;
    code += body.getValue();
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code = "ClassName("; // <-- replace with actual class name if available

	// constructor parameters
	for (const auto& arg : node.args) {
		auto typeResult = visit(*arg->type);
		if (typeResult.hasError()) return typeResult;
		code += typeResult.getValue() + " " + arg->name + ",";
	}
	if (!node.args.empty())
		code.pop_back();

	code += ")";

	// initializer list
	bool firstInit = true;
	for (const auto& arg : node.args) {
		if (!arg->initializes.empty()) {
			code += (firstInit ? " : " : ", ") + arg->initializes + "(" + arg->name + ")";
			firstInit = false;
		}
	}

	// body
	auto bodyResult = visit(*node.body);
	if (bodyResult.hasError()) return bodyResult;

	code += bodyResult.getValue();

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorStatementNode::ConstructorArgument &node) {
    std::string code;
    if(node.type) {
    	auto type = visit(*node.type);
        if(type.hasError()) return type;
        code += type.getValue() + " ";
    } else code += "auto ";
        code += node.name;
        if(node.value) {
    		auto val = visit(*node.value);
            if(val.hasError()) return val;
            code += " = " + val.getValue();
    }
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ClassDeclarationNode &node) {
    std::string code = "class " + node.className + "{";
    for(const auto &member : node.body) {
    	std::string vis;
        switch(member.visibility) {
        	case MemberVisibility::PUB: vis = "public:"; break;
            case MemberVisibility::PRI: vis = "private:"; break;
            case MemberVisibility::PRO: vis = "protected:"; break;
        }
        auto decl = visit(*member.declaration);
        if(decl.hasError()) return decl;
        code += vis + decl.getValue();
    }
    code += "};";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const UnionDeclarationNode &node) {
    std::string code = "using " + node.unionName + " = std::variant<";
    for(const auto &t : node.types) {
    	auto tcode = visit(*t);
        if(tcode.hasError()) return tcode;
        code += tcode.getValue() + ",";
    }
    if(!node.types.empty()) code.pop_back();
    code += ">;";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const IfStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code = "if(";

	Result<std::string> condition = visit(*node.condition);
	if(condition.hasError()) return condition;

	code += condition.getValue();

	code += ")";
	Result<std::string> body = visit(*node.body);
	if(body.hasError()) return body;

	code += body.getValue();

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ForStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code = "for(";

	Result<std::string> type = visit(*node.variableType);
	if(type.hasError()) return type;
	code += type.getValue() + " " + node.variableName + ":";

	Result<std::string> iterator = visit(*node.iterator);
	if(iterator.hasError()) return iterator;

	code += iterator.getValue();

	code += ")";
	Result<std::string> body = visit(*node.body);
	if(body.hasError()) return body;

	code += body.getValue();

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const WhileStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code;

	if(node.isDoWhile) {
		code += "do";
		Result<std::string> body = visit(*node.body);
		if(body.hasError()) return body;

		code += body.getValue();

		Result<std::string> condition = visit(*node.condition);
		if(condition.hasError()) return condition;
		code += "while(" + condition.getValue() + ");";
		return { code };
	}

	code += "while(";

	Result<std::string> condition = visit(*node.condition);
	if(condition.hasError()) return condition;

	code += condition.getValue();

	code += ")";
	Result<std::string> body = visit(*node.body);
	if(body.hasError()) return body;

	code += body.getValue();

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const BreakStatementNode &node) {
	return { "break;" };
}

Result<std::string> CodeGenerationVisitor::visit(const ContinueStatementNode &node) {
	return { "continue;" };
}


Result<std::string> CodeGenerationVisitor::visit(const TypeAliasNode &node) {
	std::string code = "using " + node.aliasName + "=";
	Result<std::string> type = visit(*node.targetType);
	if(type.hasError()) return type;

	code += type.getValue() + ";";

	return { code };
}


Result<std::string> CodeGenerationVisitor::visit(const BlockNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

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
    auto base = visit(*node.base);
    if(base.hasError()) return base;
    std::string code = base.getValue() + "<";
    for(const auto &p : node.params) {
    	auto pCode = visit(*p);
        if(pCode.hasError()) return pCode;
        code += pCode.getValue() + ",";
    }
    if(!node.params.empty()) code.pop_back();
    code += ">";
    return { code };
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
    std::string code = "std::tuple<";
    for(const auto &t : node.types) {
    	auto tcode = visit(*t);
        if(tcode.hasError()) return tcode;
        code += tcode.getValue() + ",";
    }
    if(!node.types.empty()) code.pop_back();
    code += ">";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const PrefixedTypeNode &node) {
    auto base = visit(*node.type);
    if(base.hasError()) return base;
    std::string code;
    
    switch(node.prefix) {
        case PrefixedTypeNode::Pointer:
            code = base.getValue() + "*";
            break;
        case PrefixedTypeNode::Owned:
            code = "std::unique_ptr<" + base.getValue() + ">";
            break;
        case PrefixedTypeNode::SharedRef:
            code = "std::shared_ptr<" + base.getValue() + ">";
            break;
        case PrefixedTypeNode::WeakRef:
            code = "std::weak_ptr<" + base.getValue() + ">";
            break;
        case PrefixedTypeNode::ImmutableRef:
            code = "const " + base.getValue() + "&";
            break;
        case PrefixedTypeNode::MutableRef:
            code = base.getValue() + "&";
            break;
        default:
            code = base.getValue();
            break;
    }
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const EnumDeclarationNode &node) {
    std::string code;
    
    if (node.isUnion) {
        // Generate tagged union using std::variant
        code = "enum class " + node.enumName + "Type { ";
        for (const auto& variant : node.variants) {
            code += variant.name + ", ";
        }
        if (!node.variants.empty()) code.pop_back(), code.pop_back();
        code += " };\n";
        
        code += "class " + node.enumName + " {\npublic:\n";
        code += "    " + node.enumName + "Type type;\n";
        code += "    std::variant<";
        
        for (const auto& variant : node.variants) {
            if (variant.fields.empty()) {
                code += "std::monostate, ";
            } else {
                code += "std::tuple<";
                for (const auto& field : variant.fields) {
                    auto fieldResult = visit(*field);
                    if (fieldResult.hasError()) return fieldResult;
                    code += fieldResult.getValue() + ", ";
                }
                if (!variant.fields.empty()) code.pop_back(), code.pop_back();
                code += ">, ";
            }
        }
        if (!node.variants.empty()) code.pop_back(), code.pop_back();
        code += "> data;\n";
        code += "};\n";
    } else {
        // Generate simple enum
        code = "enum class " + node.enumName + " { ";
        for (const auto& variant : node.variants) {
            code += variant.name + ", ";
        }
        if (!node.variants.empty()) code.pop_back(), code.pop_back();
        code += " };\n";
    }
    
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const TraitDeclarationNode &node) {
    std::string code = "// Trait " + node.traitName + "\n";
    
    // Generate generic parameters
    std::string templateParams = "template<typename T";
    for (const auto& param : node.genericParams) {
        auto paramResult = visit(*param);
        if (paramResult.hasError()) return paramResult;
        templateParams += ", typename " + paramResult.getValue();
    }
    templateParams += ">\n";
    
    code += templateParams;
    code += "concept " + node.traitName + " = requires(T t) {\n";
    
    // Generate requirements from methods
    for (const auto& method : node.methods) {
        if (method->getNodeType() == ASTNodeType::FunctionDefinition) {
            const auto* funcDef = dynamic_cast<const FunctionDefinitionNode*>(method.get());
            if (funcDef && funcDef->name) {
                auto nameResult = visit(*funcDef->name);
                if (nameResult.hasError()) continue;
                code += "    { t." + nameResult.getValue() + "() };\n";
            }
        }
    }
    
    // Add constraint if present
    if (node.constraint) {
        auto constraintResult = visit(*node.constraint);
        if (!constraintResult.hasError()) {
            code += "    // Constraint: " + constraintResult.getValue() + "\n";
        }
    }
    
    code += "};\n";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ModuleDeclarationNode &node) {
    std::string code = "// Module: " + node.moduleName + "\n";
    code += "namespace " + node.moduleName + " {\n";
    
    for (const auto& stmt : node.body) {
        auto stmtResult = visit(*stmt);
        if (stmtResult.hasError()) return stmtResult;
        code += stmtResult.getValue();
    }
    
    code += "} // namespace " + node.moduleName + "\n";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ImportStatementNode &node) {
    std::string code = "// Import " + node.moduleName;
    if (!node.importedItems.empty()) {
        code += " -> ";
        for (const auto& item : node.importedItems) {
            code += item + ", ";
        }
        code.pop_back(), code.pop_back();
    }
    code += "\n";
    code += "using namespace " + node.moduleName + ";\n";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionTypeNode &node) {
	std::string code;
	
	if (node.isClosure) {
		// Closure type: func i32 -> std::function<i32()>
		code += "std::function<";
		if (node.returnType) {
			auto retType = visit(*node.returnType);
			if (retType.hasError()) return retType;
			code += retType.getValue();
		} else {
			code += "void";
		}
		code += "()>";
	} else {
		// Function type: func(i32, i32) i32 -> std::function<i32(i32, i32)>
		code += "std::function<";
		if (node.returnType) {
			auto retType = visit(*node.returnType);
			if (retType.hasError()) return retType;
			code += retType.getValue();
		} else {
			code += "void";
		}
		code += "(";
		
		for (size_t i = 0; i < node.parameterTypes.size(); ++i) {
			auto paramType = visit(*node.parameterTypes[i]);
			if (paramType.hasError()) return paramType;
			code += paramType.getValue();
			if (i < node.parameterTypes.size() - 1) {
				code += ", ";
			}
		}
		
		code += ")>";
	}
	
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ArrayTypeNode &node) {
	std::string code;
	
	auto elemType = visit(*node.elementType);
	if (elemType.hasError()) return elemType;
	
	if (node.size) {
		// Fixed-size array: i32[10] -> std::array<int32_t, 10>
		auto sizeExpr = visit(*node.size);
		if (sizeExpr.hasError()) return sizeExpr;
		code += "std::array<" + elemType.getValue() + ", " + sizeExpr.getValue() + ">";
	} else {
		// Dynamic array: i32[] -> std::vector<int32_t>
		code += "std::vector<" + elemType.getValue() + ">";
	}
	
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const VariadicTypeNode &node) {
	// Variadic type: ...i32 -> std::vector<i32> (or similar container)
	auto baseType = visit(*node.baseType);
	if (baseType.hasError()) return baseType;
	
	return { "std::vector<" + baseType.getValue() + ">" };
}
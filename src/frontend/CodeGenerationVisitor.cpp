
// Created by User on 31/12/2024.
//
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"
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
	return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Unknown node group", node.position));
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
		case ASTNodeType::SliceExpression:
			return visit(dynamic_cast<const SliceExpressionNode&>(node));
		case ASTNodeType::MultipleIndexExpression:
			return visit(dynamic_cast<const MultipleIndexExpressionNode&>(node));
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
		
		// Pattern node cases
		case ASTNodeType::WildcardPattern:
			return visit(dynamic_cast<const WildcardPatternNode&>(node));
		case ASTNodeType::LiteralPattern:
			return visit(dynamic_cast<const LiteralPatternNode&>(node));
		case ASTNodeType::IdentifierPattern:
			return visit(dynamic_cast<const IdentifierPatternNode&>(node));
		case ASTNodeType::ArrayPattern:
			return visit(dynamic_cast<const ArrayPatternNode&>(node));
		case ASTNodeType::StructPattern:
			return visit(dynamic_cast<const StructPatternNode&>(node));
		case ASTNodeType::ConstructorPattern:
			return visit(dynamic_cast<const ConstructorPatternNode&>(node));
		case ASTNodeType::TypePattern:
			return visit(dynamic_cast<const TypePatternNode&>(node));
		case ASTNodeType::RangePattern:
			return visit(dynamic_cast<const RangePatternNode&>(node));
		
		default:
			return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Unexpected expression type", node.position));
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
		case ASTNodeType::ConstraintDeclaration:
			return visit(dynamic_cast<const ConstraintDeclarationNode&>(node));
		case ASTNodeType::ModuleDeclaration:
			return visit(dynamic_cast<const ModuleDeclarationNode&>(node));
		case ASTNodeType::ImportStatement:
			return visit(dynamic_cast<const ImportStatementNode&>(node));
		default:
			return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Unexpected statement type", node.position));
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
			return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Unexpected type", node.position));
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
	code += "#include <type_traits>\n";
	code += "#include \"runtime/ArgonRuntime.h\"\n";
	code += "\n";
	
	// Add built-in concepts
	code += "// Built-in concepts\n";
	code += "template<typename T>\n";
	code += "concept Number = std::is_arithmetic_v<T>;\n\n";
	code += "template<typename T>\n";
	code += "concept Type = true; // Any type\n\n";
	
	for (const auto& child : node.nodes) {
		auto result = visit(*child);
		if (!result.has_value()) {
			return Err<std::string>(result.error());
		}
		code += result.value();
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
			return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Invalid integer type", node.position));
	}
}

Result<std::string> CodeGenerationVisitor::visit(const FloatLiteralNode &node) {
	switch (node.type) {
		case PrimitiveType::FLOAT32:  return std::to_string(node.value.f32);
		case PrimitiveType::FLOAT64:  return std::to_string(node.value.f64);
		case PrimitiveType::FLOAT128: return std::to_string(node.value.f128);
		default:
		return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Invalid float type", node.position));
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
	if (!left.has_value()) {
		return Err<std::string>(left.error());
	}

	Result<std::string> right = visit(*node.right);
	if (!right.has_value()) {
		return Err<std::string>(right.error());
	}

	// Handle functional programming operators
	if(node.op.value == "|>") {
		// Pipe operator: left |> right becomes right(left)
		return { right.value() + "(" + left.value() + ")" };
	}
	else if(node.op.value == "||>") {
		// Map pipe: left ||> right becomes ArgonLang::Runtime::map_pipe
		return { "ArgonLang::Runtime::map_pipe(" + left.value() + ", " + right.value() + ")" + (isStatementContext? ";": "") };
	}
	else if(node.op.value == "|") {
		// Filter operator: left | right becomes ArgonLang::Runtime::filter
		return { "ArgonLang::Runtime::filter(" + left.value() + ", " + right.value() + ")" + (isStatementContext? ";": "") };
	}
	else if(node.op.value == "&") {
		// In constraint context, & is bitwise AND; otherwise it's the map operator
		if (isConstraintContext) {
			return { "(" + left.value() + " & " + right.value() + ")" };
		} else {
			// Map operator: left & right becomes ArgonLang::Runtime::map
			return { "ArgonLang::Runtime::map(" + left.value() + ", " + right.value() + ")" + (isStatementContext? ";": "") };
		}
	}
	else if(node.op.value == "^") {
		// Reduce operator: left ^ right becomes ArgonLang::Runtime::reduce
		return { "ArgonLang::Runtime::reduce(" + left.value() + ", " + right.value() + ")" + (isStatementContext? ";": "") };
	}
	else if(node.op.value == "^^") {
		// Accumulate range: left ^^ right becomes ArgonLang::Runtime::accumulate
		return { "ArgonLang::Runtime::accumulate(" + left.value() + ", " + right.value() + ")" + (isStatementContext? ";": "") };
	}
	else if(node.op.value == "to") {
		// Range operator: handled by ToExpressionNode, but if it appears here as binary op
		return { "std::ranges::iota_view(" + left.value() + ", " + right.value() + ")" + (isStatementContext? ";": "") };
	}
	else {
		// Regular binary operators
		return { left.value() + " " + node.op.value + " " + right.value() };
	}
}

Result<std::string> CodeGenerationVisitor::visit(const UnaryExpressionNode &node) {
	Result<std::string> operand = visit(*node.operand);
	if (!operand.has_value()) {
		return Err<std::string>(operand.error());
	}

	// Handle special unary operators
	if(node.op.value == "$") {
		// Iterator syntax: $arr becomes {arr.begin(), arr.end()}
		return { "std::make_pair(" + operand.value() + ".begin(), " + operand.value() + ".end())" };
	}
	else if(node.op.value == "~") {
		// Move/ownership operator: ~value becomes std::move(value)
		return { "std::move(" + operand.value() + ")" };
	}
	else if(node.op.value == "await") {
		// Await operator: await future becomes ArgonLang::Runtime::await(future)
		return { "ArgonLang::Runtime::await(std::move(" + operand.value() + "))" + (isStatementContext? ";": "") };
	}
	else if(node.op.value == "&") {
		// Const reference (immutable reference)
		return { "&" + operand.value() };
	}
	else if(node.op.value == "&&") {
		// Mutable reference (normal reference)
		return { "&" + operand.value() };
	}
	else {
		// Regular unary operators
		return { node.op.value + operand.value() };
	}
}


Result<std::string> CodeGenerationVisitor::visit(const FunctionCallExpressionNode &node) {
	Result<std::string> functionName = visit(*node.function);
	if (!functionName.has_value()) {
		return Err<std::string>(functionName.error());
	}

	std::string code = functionName.value();
	
	// Add generic type arguments if present: func<Type1, Type2>
	if (!node.genericTypeArgs.empty()) {
		code += "<";
		for (size_t i = 0; i < node.genericTypeArgs.size(); i++) {
			Result<std::string> typeResult = visit(*node.genericTypeArgs[i]);
			if (!typeResult.has_value()) {
				return Err<std::string>(typeResult.error());
			}
			
			if (i > 0) code += ", ";
			code += typeResult.value();
		}
		code += ">";
	}
	
	code += "(";

	for (int i = 0; i < node.arguments.size(); i++) {
		const auto &arg = node.arguments[i];
		Result<std::string> argResult = visit(*arg);
		
		if (!argResult.has_value()) {
			return Err<std::string>(argResult.error());
		}
		
		code += argResult.value();
		if (i != node.arguments.size() - 1)
			code += ", ";
	}
	code += ")";

	if(this->isStatementContext) code += ";";
	
	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const MemberAccessExpressionNode &node) {
	Result<std::string> parent = visit(*node.parent);
	if (!parent.has_value()) {
		return Err<std::string>(parent.error());
	}
	Result<std::string> member = visit(*node.member);
	if (!member.has_value()) {
		return Err<std::string>(member.error());
	}

	return { parent.value() + node.accessType.value + member.value() };
}

Result<std::string> CodeGenerationVisitor::visit(const ToExpressionNode &node) {
	Result<std::string> lowerBound = visit(*node.lowerBound);
	if (!lowerBound.has_value()) {
		return Err<std::string>(lowerBound.error());
	}

	Result<std::string> upperBound = visit(*node.upperBound);
	if (!upperBound.has_value()) {
		return Err<std::string>(upperBound.error());
	}

	return {
		"std::ranges::iota_view(" + lowerBound.value() + "," + upperBound.value() + (node.isInclusive? "+1": "") + ")"
	};
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionArgument &node) {
	std::string type = "auto";
	if(node.type) {
		Result<std::string> typeResult = visit(*node.type);
		if (!typeResult.has_value()) {
			return Err<std::string>(typeResult.error());
		}
		type = typeResult.value();
	}

	return { type + " " + node.name };
}

Result<std::string> CodeGenerationVisitor::visit(const LambdaExpressionNode &node) {
	std::string code = "[&](";

	for (const auto& param : node.parameters) {
		auto paramResult = visit(*param);
		if (!paramResult.has_value()) {
			return Err<std::string>(paramResult.error());
		}
		code += paramResult.value() + ",";
	}

	if (!node.parameters.empty()) code.pop_back();
	code += ")";

	Result<std::string> body = visit(*node.body);
	if (!body.has_value()) {
		return Err<std::string>(body.error());
	}

	if(node.body->getNodeType() != ASTNodeType::Block)
		code += "{" + body.value() + "}";
	else
		code += body.value();

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ComparisonExpressionNode &node) {
	Result<std::string> left = visit(*node.left);
	if (!left.has_value()) {
		return Err<std::string>(left.error());
	}

	Result<std::string> right = visit(*node.right);
	if (!right.has_value()) {
		return Err<std::string>(right.error());
	}

	return { left.value() + node.op.value + right.value() };
}

Result<std::string> CodeGenerationVisitor::visit(const AssignmentExpressionNode &node) {
	Result<std::string> left = visit(*node.left);
	if (!left.has_value()) {
		return Err<std::string>(left.error());
	}

	Result<std::string> right = visit(*node.right);
	if (!right.has_value()) {
		return Err<std::string>(right.error());
	}

	std::string code = left.value() + node.op.value + right.value();
	
	// Add semicolon when used as a statement
	if(this->isStatementContext) code += ";";
	
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const IndexExpressionNode &node) {
	Result<std::string> array = visit(*node.array);
	if (!array.has_value()) {
		return Err<std::string>(array.error());
	}

	Result<std::string> index = visit(*node.index);
	if (!index.has_value()) {
		return Err<std::string>(index.error());
	}

	return { array.value() + "[" + index.value() + "]" };
}

Result<std::string> CodeGenerationVisitor::visit(const SliceExpressionNode &node) {
	Result<std::string> array = visit(*node.array);
	if (!array.has_value()) {
		return Err<std::string>(array.error());
	}

	Result<std::string> start = visit(*node.start);
	if (!start.has_value()) {
		return Err<std::string>(start.error());
	}

	Result<std::string> end = visit(*node.end);
	if (!end.has_value()) {
		return Err<std::string>(end.error());
	}

	// Generate slice using std::ranges::subrange or similar
	std::string endExpr = node.isInclusive ? "(" + end.value() + " + 1)" : end.value();
	return { "std::ranges::subrange(" + array.value() + ".begin() + " + start.value() + 
	         ", " + array.value() + ".begin() + " + endExpr + ")" };
}

Result<std::string> CodeGenerationVisitor::visit(const MultipleIndexExpressionNode &node) {
	Result<std::string> array = visit(*node.array);
	if (!array.has_value()) {
		return Err<std::string>(array.error());
	}

	std::string code = "{";
	for (size_t i = 0; i < node.indices.size(); ++i) {
		Result<std::string> index = visit(*node.indices[i]);
		if (!index.has_value()) {
			return Err<std::string>(index.error());
		}
		
		code += array.value() + "[" + index.value() + "]";
		if (i < node.indices.size() - 1) {
			code += ", ";
		}
	}
	code += "}";
	
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const MatchBranch &node) {
    std::string pattern;
    if(node.pattern) {
    	auto p = visit(*node.pattern);
        if (!p.has_value()) {
            return Err<std::string>(p.error());
        }
        pattern = p.value();
    }

    auto body = visit(*node.body);
    if (!body.has_value()) {
        return Err<std::string>(body.error());
    }

    // Handle guard condition if present
    std::string guardCondition;
    if(node.condition) {
        auto guard = visit(*node.condition);
        if (!guard.has_value()) {
            return Err<std::string>(guard.error());
        }
        guardCondition = guard.value();
    }

	std::string code;
	std::string patternCondition;
	
	// Generate pattern matching condition using runtime utilities
	if (pattern == "_") {
		patternCondition = "ArgonLang::Runtime::match_wildcard(__match_val)";
	} else if (pattern.find("to") != std::string::npos && pattern.find("=") != std::string::npos) {
		// Handle range patterns like "1 to= 10"
		size_t toPos = pattern.find(" to");
		if (toPos != std::string::npos) {
			std::string start = pattern.substr(0, toPos);
			std::string end = pattern.substr(toPos + 4); // Skip " to="
			patternCondition = "ArgonLang::Runtime::match_range(__match_val, " + start + ", " + end + ", true)";
		} else {
			patternCondition = "ArgonLang::Runtime::match_value(__match_val, " + pattern + ")";
		}
	} else if (pattern.find("to") != std::string::npos) {
		// Handle exclusive range patterns like "1 to 10"
		size_t toPos = pattern.find(" to ");
		if (toPos != std::string::npos) {
			std::string start = pattern.substr(0, toPos);
			std::string end = pattern.substr(toPos + 4);
			patternCondition = "ArgonLang::Runtime::match_range(__match_val, " + start + ", " + end + ", false)";
		} else {
			patternCondition = "ArgonLang::Runtime::match_value(__match_val, " + pattern + ")";
		}
		} else {
		// Check if this is an identifier pattern (should capture, not compare)
		// Identifier patterns should always match and capture the value
		if(node.pattern && node.pattern->getNodeType() == ASTNodeType::IdentifierPattern) {
			patternCondition = "ArgonLang::Runtime::match_wildcard(__match_val)"; // Always matches
		} else {
			patternCondition = "ArgonLang::Runtime::match_value(__match_val, " + pattern + ")";
		}
	}
	
	// Handle variable binding for identifier patterns in guard conditions
	if(node.pattern && node.pattern->getNodeType() == ASTNodeType::IdentifierPattern && !guardCondition.empty()) {
		const IdentifierPatternNode* identifierPattern = dynamic_cast<const IdentifierPatternNode*>(node.pattern.get());
		if(identifierPattern) {
			// Use C++17 structured binding in if statement: if(auto var = value; condition)
			code = "if(auto " + identifierPattern->name + " = __match_val; " + guardCondition + ") {";
	} else {
			code = "if(" + patternCondition + " && (" + guardCondition + ")) {";
		}
	} else if(!guardCondition.empty()) {
		code = "if(" + patternCondition + " && (" + guardCondition + ")) {";
	} else {
		code = "if(" + patternCondition + ") {";
	}

	if((*node.body).getNodeGroup() == ASTNodeGroup::Expression) {
		code += "return " + body.value() + ";}";
	} else {
		code += body.value() + "}";
	}

    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const MatchExpressionNode &node) {
		ScopedStatementContext scoped(this->isStatementContext, false);
        auto value = visit(*node.value);
        if (!value.has_value()) {
            return Err<std::string>(value.error());
        }

        std::string code = "([&]() { auto __match_val = " + value.value() + ";";
        bool first = true;
        for(const auto &branch : node.branches) {
            auto branchCode = visit(*branch);
            if (!branchCode.has_value()) {
                return Err<std::string>(branchCode.error());
            }
            if(!first) code += " else ";
            code += branchCode.value();
            first = false;
        }
        code += "})()";
        return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const TernaryExpressionNode &node) {
        auto cond = visit(*node.condition);
        if (!cond.has_value()) {
            return Err<std::string>(cond.error());
        }
        auto t = visit(*node.trueBranch);
        if (!t.has_value()) {
            return Err<std::string>(t.error());
        }
        auto f = visit(*node.falseBranch);
        if (!f.has_value()) {
            return Err<std::string>(f.error());
        }

        return { cond.value() + " ? " + t.value() + " : " + f.value() };

}

Result<std::string> CodeGenerationVisitor::visit(const ParallelExpressionNode &node) {
        // Temporarily disable statement context to avoid extra semicolons
        bool originalContext = this->isStatementContext;
        ScopedStatementContext scoped(this->isStatementContext, false);
        auto expr = visit(*node.statementNode);
        if (!expr.has_value()) {
            return Err<std::string>(expr.error());
        }
        
        // Use ArgonLang runtime library for parallel execution
        if(node.statementNode->getNodeGroup() == ASTNodeGroup::Statement) {
            // For statements, wrap in lambda that returns 0
            return { "ArgonLang::Runtime::par([&]() { " + expr.value() + "; return 0; })" + (originalContext? ";": "") };
        } else {
            // For expressions, wrap in lambda that returns the expression value
            return { "ArgonLang::Runtime::par([&]() { return " + expr.value() + "; })" + (originalContext? ";": "") };
        }
}

Result<std::string> CodeGenerationVisitor::visit(const StructField &node) {
        std::string code = "." + node.name;
        if(node.value) {
                auto val = visit(*node.value);
                if (!val.has_value()) {
                    return Err<std::string>(val.error());
                }
                code += " = " + val.value();
        }
        return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const StructExpressionNode &node) {
        // Generate a unique struct name based on position
        std::string structName = "AnonymousStruct_" + std::to_string(node.position.line) + "_" + std::to_string(node.position.column);
        
        // Generate an immediately invoked lambda that creates and returns the struct
        std::string code = "([&]() {";
        
        // Define the struct type
        code += "struct " + structName + " {";
        for(const auto &field : node.fields) {
                if(field.type) {
                        auto typeCode = visit(*field.type);
                        if (!typeCode.has_value()) {
                            return Err<std::string>(typeCode.error());
                        }
                        code += typeCode.value() + " " + field.name + ";";
                } else {
                        // If no explicit type, try to infer from the value
                        // For now, we'll use a generic approach - this could be improved with type inference
                        code += "decltype(";
                        if(field.value) {
                                auto valueCode = visit(*field.value);
                                if (!valueCode.has_value()) {
                                    return Err<std::string>(valueCode.error());
                                }
                                code += valueCode.value();
                        } else {
                                code += "0"; // Default to int if no value provided
                        }
                        code += ") " + field.name + ";";
                }
        }
        code += "};";
        
        // Create and initialize an instance
        code += structName + " instance{};";
        for(const auto &field : node.fields) {
                if(field.value) {
                        auto valueCode = visit(*field.value);
                        if (!valueCode.has_value()) {
                            return Err<std::string>(valueCode.error());
                        }
                        code += "instance." + field.name + " = " + valueCode.value() + ";";
                }
        }
        
        // Return the instance
        code += "return instance;";
        code += "}())";
        
        return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const RangeExpressionNode &node) {
	std::string code = "vector{";

	for(const auto& i: node.range) {
		Result<std::string> item = visit(*i);
		if (!item.has_value()) {
			return Err<std::string>(item.error());
		}
		code += item.value() + ",";
	}

	code.pop_back();
	code += "}";
	return { code };
}


Result<std::string> CodeGenerationVisitor::visit(const VariableDeclarationNode &node) {
	std::string code;
	
	// Handle compound destructuring declarations (def [first], rest = arr)
	if(!node.compoundPatterns.empty()) {
		if(!node.value) {
			return Err<std::string>(create_parse_error(ErrorType::InvalidStatement, "Compound destructuring declaration must have a value", node.position));
		}
		
		ScopedStatementContext scoped(this->isStatementContext, false);
		Result<std::string> value = visit(*node.value);
		if (!value.has_value()) {
			return Err<std::string>(value.error());
		}
		
		// Generate unique temporary variable for the value using position
		std::string tempVar = "__compound_temp_" + std::to_string(node.position.line) + "_" + std::to_string(node.position.column);
		code += "auto " + tempVar + " = " + value.value() + ";";
		
		// Generate compound destructuring assignments
		Result<std::string> compoundDestructuring = generateCompoundDestructuring(node.compoundPatterns, tempVar);
		if (!compoundDestructuring.has_value()) {
			return Err<std::string>(compoundDestructuring.error());
		}
		
		code += compoundDestructuring.value();
		return { code };
	}
	
	// Handle single pattern destructuring declarations
	if(node.pattern) {
		if(!node.value) {
			return Err<std::string>(create_parse_error(ErrorType::InvalidStatement, "Destructuring declaration must have a value", node.position));
		}
		
		ScopedStatementContext scoped(this->isStatementContext, false);
		Result<std::string> value = visit(*node.value);
		if (!value.has_value()) {
			return Err<std::string>(value.error());
		}
		
		// Generate unique temporary variable for the value using position
		std::string tempVar = "__destructure_temp_" + std::to_string(node.position.line) + "_" + std::to_string(node.position.column);
		code += "auto " + tempVar + " = " + value.value() + ";";
		
		// Generate destructuring assignments
		Result<std::string> destructuring = generateDestructuring(node.pattern.get(), tempVar);
		if (!destructuring.has_value()) {
			return Err<std::string>(destructuring.error());
		}
		
		code += destructuring.value();
		return { code };
	}
	
	// Handle simple declarations
	if(node.type) {
		Result<std::string> type = visit(*node.type);
		if (!type.has_value()) {
			return Err<std::string>(type.error());
		}
		code += type.value() + " ";
	} else code += "auto ";

	code += node.name;

	if (node.value) {
		ScopedStatementContext scoped(this->isStatementContext, false);
		Result<std::string> value = visit(*node.value);
		if (!value.has_value()) {
			return Err<std::string>(value.error());
		}
		code += " = " + value.value();
	}

	code += ";";

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionDeclarationNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	auto nameResult = visit(*node.name);
	if (!nameResult.has_value()) {
		return Err<std::string>(nameResult.error());
	}
	std::string functionName = nameResult.value();

	std::string returnType = functionName == "main"? "int": "auto";
	if (node.returnType) {
		auto returnTypeResult = visit(*node.returnType);
		if (!returnTypeResult.has_value()) {
			return Err<std::string>(returnTypeResult.error());
		}
		returnType = returnTypeResult.value();
	}

	std::string code;
	
	// Generate template parameters if present
	auto genericResult = generateGenericParameters(node.genericParams);
	if (!genericResult.has_value()) {
		return Err<std::string>(genericResult.error());
	}
	code += genericResult.value();

	code += returnType + " " + functionName + "(";

	for (const auto& arg : node.args) {
		auto argResult = visit(*arg);
		if (!argResult.has_value()) {
			return Err<std::string>(argResult.error());
		}
		code += argResult.value() + ",";
	}
	if (!node.args.empty()) {
		code.pop_back();
	}

	code += ")";

	if (node.body->getNodeType() != ASTNodeType::Block) {
		auto bodyResult = visit(*node.body);
		if (!bodyResult.has_value()) {
			return Err<std::string>(bodyResult.error());
		}
		code += " { return " + bodyResult.value() + "; }";
	} else {
		auto bodyResult = visit(*node.body);
		if (!bodyResult.has_value()) {
			return Err<std::string>(bodyResult.error());
		}
		code += bodyResult.value();
	}

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const FunctionDefinitionNode &node) {
	Result<std::string> functionName = visit(*node.name);
	if (!functionName.has_value()) {
		return Err<std::string>(functionName.error());
	}
	
	std::string returnType = "int";
	if (node.returnType) {
		auto returnTypeResult = visit(*node.returnType);
		if (!returnTypeResult.has_value()) {
			return Err<std::string>(returnTypeResult.error());
		}
		returnType = returnTypeResult.value();
	}

	std::string code;
	
	// Generate template parameters if present
	auto genericResult = generateGenericParameters(node.genericParams);
	if (!genericResult.has_value()) {
		return Err<std::string>(genericResult.error());
	}
	code += genericResult.value();

	code += returnType + " " + functionName.value() + "(";
	for (const auto &arg: node.args) {
		Result<std::string> type = visit(*arg->type);
		if (!type.has_value()) {
			return Err<std::string>(type.error());
		}

		code += type.value() + " " + arg->name + ",";
	}
	if (!node.args.empty()) {
		code.pop_back();
	}

	code += ");";
	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const ReturnStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, false);
	Result<std::string> returnValue = visit(*node.returnExpression);
	if (!returnValue.has_value()) {
		return Err<std::string>(returnValue.error());
	}

	return { "return " + returnValue.value() + ";" };
}

Result<std::string> CodeGenerationVisitor::visit(const YieldStatementNode &node) {
	Result<std::string> yieldValue = visit(*node.expressionNode);
	if (!yieldValue.has_value()) {
		return Err<std::string>(yieldValue.error());
	}

	return { "co_yield " + yieldValue.value() + ";" };
}

Result<std::string> CodeGenerationVisitor::visit(const ImplStatementNode &node) {
    std::string code = "// impl " + node.className;
    auto body = visit(*node.body);
    if (!body.has_value()) {
        return Err<std::string>(body.error());
    }
    code += body.value();
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code = "ClassName("; // <-- replace with actual class name if available

	// constructor parameters
	for (const auto& arg : node.args) {
		auto typeResult = visit(*arg->type);
		if (!typeResult.has_value()) {
			return Err<std::string>(typeResult.error());
		}
		code += typeResult.value() + " " + arg->name + ",";
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
	if (!bodyResult.has_value()) {
		return Err<std::string>(bodyResult.error());
	}

	code += bodyResult.value();

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorStatementNode::ConstructorArgument &node) {
    std::string code;
    if(node.type) {
    	auto type = visit(*node.type);
        if (!type.has_value()) {
            return Err<std::string>(type.error());
        }
        code += type.value() + " ";
    } else code += "auto ";
        code += node.name;
        if(node.value) {
    		auto val = visit(*node.value);
            if (!val.has_value()) {
                return Err<std::string>(val.error());
            }
            code += " = " + val.value();
    }
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ClassDeclarationNode &node) {
    std::string code;
    
    // Generate template parameters if present
    if (!node.genericParams.empty()) {
        auto templateResult = generateGenericParameters(node.genericParams);
        if (!templateResult.has_value()) {
            return Err<std::string>(templateResult.error());
        }
        code += templateResult.value();
    }
    
    code += "class " + node.className + "{";
    for(const auto &member : node.body) {
    	std::string vis;
        switch(member.visibility) {
        	case MemberVisibility::PUB: vis = "public:"; break;
            case MemberVisibility::PRI: vis = "private:"; break;
            case MemberVisibility::PRO: vis = "protected:"; break;
        }
        auto decl = visit(*member.declaration);
        if (!decl.has_value()) {
            return Err<std::string>(decl.error());
        }
        code += vis + decl.value();
    }
    code += "};";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const UnionDeclarationNode &node) {
    std::string code = "using " + node.unionName + " = std::variant<";
    for(const auto &t : node.types) {
    	auto tcode = visit(*t);
        if (!tcode.has_value()) {
            return Err<std::string>(tcode.error());
        }
        code += tcode.value() + ",";
    }
    if(!node.types.empty()) code.pop_back();
    code += ">;";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const IfStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code = "if(";

	Result<std::string> condition = visit(*node.condition);
	if (!condition.has_value()) {
		return Err<std::string>(condition.error());
	}

	code += condition.value();

	code += ")";
	Result<std::string> body = visit(*node.body);
	if (!body.has_value()) {
		return Err<std::string>(body.error());
	}

	code += body.value();

	// Handle else branch if present
	if(node.elseBranch != nullptr) {
		code += "else";
		Result<std::string> elseBranch = visit(*node.elseBranch);
		if (!elseBranch.has_value()) {
			return Err<std::string>(elseBranch.error());
		}
		code += elseBranch.value();
	}

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ForStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	Result<std::string> iterator = visit(*node.iterator);
	if (!iterator.has_value()) {
		return Err<std::string>(iterator.error());
	}
	
	std::string iteratorCode = iterator.value();
	std::string code;
	
	// Check if this is a range expression (contains "std::ranges::iota_view")
	if(iteratorCode.find("std::ranges::iota_view") != std::string::npos) {
		// Range-based for loop: for(i -> 0 to 10)
		if(node.variableType) {
	Result<std::string> type = visit(*node.variableType);
			if (!type.has_value()) {
				return Err<std::string>(type.error());
			}
			code += "for(" + type.value() + " " + node.variableName + " : " + iteratorCode + ")";
		} else {
			code += "for(auto " + node.variableName + " : " + iteratorCode + ")";
		}
	}
	// Check if this is an iterator syntax ($array) or iterator pair variable
	else if(iteratorCode.find("begin()") != std::string::npos && iteratorCode.find("end()") != std::string::npos) {
		// Direct iterator syntax: for(i -> $arr)
		std::string containerVar = "__for_container_" + std::to_string(node.position.line) + "_" + std::to_string(node.position.column);
		code += "auto " + containerVar + " = " + iteratorCode + ";";
		
		if(node.variableType) {
			Result<std::string> type = visit(*node.variableType);
			if (!type.has_value()) {
				return Err<std::string>(type.error());
			}
			code += "for(auto __it = " + containerVar + ".first; __it != " + containerVar + ".second; ++__it) {";
			code += type.value() + " " + node.variableName + " = *__it;";
		} else {
			code += "for(auto __it = " + containerVar + ".first; __it != " + containerVar + ".second; ++__it) {";
			code += "auto " + node.variableName + " = *__it;";
		}
		
		Result<std::string> body = visit(*node.body);
		if (!body.has_value()) {
			return Err<std::string>(body.error());
		}
		
		// Handle body - if it's a block, extract its content, otherwise wrap it
		std::string bodyCode = body.value();
		if(bodyCode.front() == '{' && bodyCode.back() == '}') {
			// Remove outer braces and add the content
			code += bodyCode.substr(1, bodyCode.length() - 2);
		} else {
			code += bodyCode;
		}
		code += "}";
		
		return { code };
	}
	// Check if this might be an iterator pair variable
	else if(iteratorCode.find("std::make_pair") != std::string::npos || 
	        (iteratorCode.find('.') == std::string::npos && iteratorCode.find('[') == std::string::npos && iteratorCode.find('(') == std::string::npos)) {
		// Iterator pair variable: for(i -> iterator)
		if(node.variableType) {
			Result<std::string> type = visit(*node.variableType);
			if (!type.has_value()) {
				return Err<std::string>(type.error());
			}
			code += "for(auto __it = " + iteratorCode + ".first; __it != " + iteratorCode + ".second; ++__it) {";
			code += type.value() + " " + node.variableName + " = *__it;";
		} else {
			code += "for(auto __it = " + iteratorCode + ".first; __it != " + iteratorCode + ".second; ++__it) {";
			code += "auto " + node.variableName + " = *__it;";
		}
		
	Result<std::string> body = visit(*node.body);
		if (!body.has_value()) {
			return Err<std::string>(body.error());
		}
		
		// Handle body - if it's a block, extract its content, otherwise wrap it
		std::string bodyCode = body.value();
		if(bodyCode.front() == '{' && bodyCode.back() == '}') {
			// Remove outer braces and add the content
			code += bodyCode.substr(1, bodyCode.length() - 2);
		} else {
			code += bodyCode;
		}
		code += "}";
		
		return { code };
	}
	else {
		// Collection-based for loop: for(i -> collection)
		if(node.variableType) {
			Result<std::string> type = visit(*node.variableType);
			if (!type.has_value()) {
				return Err<std::string>(type.error());
			}
			code += "for(" + type.value() + " " + node.variableName + " : " + iteratorCode + ")";
		} else {
			code += "for(auto " + node.variableName + " : " + iteratorCode + ")";
		}
	}
	
	Result<std::string> body = visit(*node.body);
	if (!body.has_value()) {
		return Err<std::string>(body.error());
	}

	code += body.value();

	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const WhileStatementNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code;

	if(node.isDoWhile) {
		code += "do";
		Result<std::string> body = visit(*node.body);
		if (!body.has_value()) {
			return Err<std::string>(body.error());
		}

		code += body.value();

		Result<std::string> condition = visit(*node.condition);
		if (!condition.has_value()) {
			return Err<std::string>(condition.error());
		}
		code += "while(" + condition.value() + ");";
		return { code };
	}

	code += "while(";

	Result<std::string> condition = visit(*node.condition);
	if (!condition.has_value()) {
		return Err<std::string>(condition.error());
	}

	code += condition.value();

	code += ")";
	Result<std::string> body = visit(*node.body);
	if (!body.has_value()) {
		return Err<std::string>(body.error());
	}

	code += body.value();

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
	if (!type.has_value()) {
		return Err<std::string>(type.error());
	}

	code += type.value() + ";";

	return { code };
}


Result<std::string> CodeGenerationVisitor::visit(const BlockNode &node) {
	ScopedStatementContext scoped(this->isStatementContext, true);

	std::string code = "{";
	
	// Add scope management for parallel execution
	code += "ARGON_SCOPE_BEGIN();";

	for(const auto& statement: node.body) {
		Result<std::string> statementCode = visit(*statement);
		if (!statementCode.has_value()) {
			return Err<std::string>(statementCode.error());
		}
		code += statementCode.value();
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
    if (!base.has_value()) {
        return Err<std::string>(base.error());
    }
    std::string code = base.value() + "<";
    for(const auto &p : node.params) {
    	auto pCode = visit(*p);
        if (!pCode.has_value()) {
            return Err<std::string>(pCode.error());
        }
        code += pCode.value() + ",";
    }
    if(!node.params.empty()) code.pop_back();
    code += ">";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const SumTypeNode &node) {
	std::string code = "std::variant<";
	for(const auto& type: node.types) {
		Result<std::string> typeCode = visit(*type);
		if (!typeCode.has_value()) {
			return Err<std::string>(typeCode.error());
		}

		code += typeCode.value() + ",";
	}

	if (!node.types.empty()) {
		code.pop_back();
	}
	code += ">";

	return code;
}

Result<std::string> CodeGenerationVisitor::visit(const IntersectionTypeNode &node) {
    // Intersection types are for compile-time validation only (AnalysisVisitor)
    // Generate the base type (first type) since safety is verified at compile time
    if (node.types.empty()) {
        return { "void" }; // Fallback for empty intersection
    }
    
    // Use the first type as the runtime representation
    return visit(*node.types[0]);
}

Result<std::string> CodeGenerationVisitor::visit(const PrefixedTypeNode &node) {
    auto base = visit(*node.type);
    if (!base.has_value()) {
        return Err<std::string>(base.error());
    }
    std::string code;
    
    switch(node.prefix) {
        case PrefixedTypeNode::Pointer:
            code = base.value() + "*";
            break;
        case PrefixedTypeNode::Owned:
            code = "std::unique_ptr<" + base.value() + ">";
            break;
        case PrefixedTypeNode::SharedRef:
            code = "std::shared_ptr<" + base.value() + ">";
            break;
        case PrefixedTypeNode::WeakRef:
            code = "std::weak_ptr<" + base.value() + ">";
            break;
        case PrefixedTypeNode::ImmutableRef:
            code = "const " + base.value() + "&";
            break;
        case PrefixedTypeNode::MutableRef:
            code = base.value() + "&";
            break;
        default:
            code = base.value();
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
                    if (!fieldResult.has_value()) {
                        return Err<std::string>(fieldResult.error());
                    }
                    code += fieldResult.value() + ", ";
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



Result<std::string> CodeGenerationVisitor::visit(const ConstraintDeclarationNode &node) {
    std::string code = "// Constraint " + node.constraintName + "\n";
    
    // Generate template parameters if present
    auto genericResult = generateGenericParameters(node.genericParams);
    if (!genericResult.has_value()) {
        return Err<std::string>(genericResult.error());
    }
    code += genericResult.value();
    
    // Generate constraint as a concept or type alias
    code += "concept " + node.constraintName + " = ";
    
    // Set constraint context for proper operator handling
    bool previousConstraintContext = isConstraintContext;
    isConstraintContext = true;
    
    auto constraintResult = visit(*node.constraintExpression);
    
    // Restore previous context
    isConstraintContext = previousConstraintContext;
    
    if (!constraintResult.has_value()) {
        return Err<std::string>(constraintResult.error());
    }
    
    code += constraintResult.value() + ";\n";
    return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ModuleDeclarationNode &node) {
    std::string code = "// Module: " + node.moduleName + "\n";
    code += "namespace " + node.moduleName + " {\n";
    
    for (const auto& stmt : node.body) {
        auto stmtResult = visit(*stmt);
        if (!stmtResult.has_value()) {
            return Err<std::string>(stmtResult.error());
        }
        code += stmtResult.value();
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
			if (!retType.has_value()) {
				return Err<std::string>(retType.error());
			}
			code += retType.value();
		} else {
			code += "void";
		}
		code += "()>";
	} else {
		// Function type: func(i32, i32) i32 -> std::function<i32(i32, i32)>
		code += "std::function<";
		if (node.returnType) {
			auto retType = visit(*node.returnType);
			if (!retType.has_value()) {
				return Err<std::string>(retType.error());
			}
			code += retType.value();
		} else {
			code += "void";
		}
		code += "(";
		
		for (size_t i = 0; i < node.parameterTypes.size(); ++i) {
			auto paramType = visit(*node.parameterTypes[i]);
			if (!paramType.has_value()) {
				return Err<std::string>(paramType.error());
			}
			code += paramType.value();
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
	if (!elemType.has_value()) {
		return Err<std::string>(elemType.error());
	}
	
	if (node.size) {
		// Fixed-size array: i32[10] -> std::array<int32_t, 10>
		auto sizeExpr = visit(*node.size);
		if (!sizeExpr.has_value()) {
			return Err<std::string>(sizeExpr.error());
		}
		code += "std::array<" + elemType.value() + ", " + sizeExpr.value() + ">";
	} else {
		// Dynamic array: i32[] -> std::vector<int32_t>
		code += "std::vector<" + elemType.value() + ">";
	}
	
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const VariadicTypeNode &node) {
	// Variadic type: ...i32 -> std::vector<i32> (or similar container)
	auto baseType = visit(*node.baseType);
	if (!baseType.has_value()) {
		return Err<std::string>(baseType.error());
	}
	
	return { "std::vector<" + baseType.value() + ">" };
}

// Pattern node implementations
Result<std::string> CodeGenerationVisitor::visit(const PatternNode &node) {
	// Dispatch to specific pattern types
	switch(node.getNodeType()) {
		case ASTNodeType::WildcardPattern:
			return visit(dynamic_cast<const WildcardPatternNode&>(node));
		case ASTNodeType::LiteralPattern:
			return visit(dynamic_cast<const LiteralPatternNode&>(node));
		case ASTNodeType::IdentifierPattern:
			return visit(dynamic_cast<const IdentifierPatternNode&>(node));
		case ASTNodeType::ArrayPattern:
			return visit(dynamic_cast<const ArrayPatternNode&>(node));
		case ASTNodeType::StructPattern:
			return visit(dynamic_cast<const StructPatternNode&>(node));
		case ASTNodeType::ConstructorPattern:
			return visit(dynamic_cast<const ConstructorPatternNode&>(node));
		case ASTNodeType::TypePattern:
			return visit(dynamic_cast<const TypePatternNode&>(node));
		case ASTNodeType::RangePattern:
			return visit(dynamic_cast<const RangePatternNode&>(node));
		default:
			return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Unknown pattern type", node.position));
	}
}

Result<std::string> CodeGenerationVisitor::visit(const WildcardPatternNode &node) {
	// Wildcard pattern "_" matches anything
	return { "_" };
}

Result<std::string> CodeGenerationVisitor::visit(const LiteralPatternNode &node) {
	// Generate code for the literal expression
	return visit(*node.literal);
}

Result<std::string> CodeGenerationVisitor::visit(const IdentifierPatternNode &node) {
	// Identifier pattern captures the value
	return { node.name };
}

Result<std::string> CodeGenerationVisitor::visit(const ArrayPatternNode &node) {
	// Array pattern [a, b, c] - would need destructuring logic
	std::string code = "[";
	bool first = true;
	for (const auto& pattern : node.elements) {
		if (!first) code += ", ";
		auto patternCode = visit(*pattern);
		if (!patternCode.has_value()) {
			return Err<std::string>(patternCode.error());
		}
		code += patternCode.value();
		first = false;
	}
	code += "]";
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const StructPatternNode &node) {
	// Struct pattern {field1: pattern1, field2: pattern2}
	std::string code = "{";
	bool first = true;
	for (const auto& field : node.fields) {
		if (!first) code += ", ";
		code += field.first + ": ";
		auto patternCode = visit(*field.second);
		if (!patternCode.has_value()) {
			return Err<std::string>(patternCode.error());
		}
		code += patternCode.value();
		first = false;
	}
	code += "}";
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const ConstructorPatternNode &node) {
	// Constructor pattern Type(args...)
	std::string code = node.constructorName + "(";
	bool first = true;
	for (const auto& arg : node.arguments) {
		if (!first) code += ", ";
		auto argCode = visit(*arg);
		if (!argCode.has_value()) {
			return Err<std::string>(argCode.error());
		}
		code += argCode.value();
		first = false;
	}
	code += ")";
	return { code };
}

Result<std::string> CodeGenerationVisitor::visit(const TypePatternNode &node) {
	// Type pattern - check if value is of specific type
	auto typeCode = visit(*node.type);
	if (!typeCode.has_value()) {
		return Err<std::string>(typeCode.error());
	}
	return { typeCode.value() };
}

Result<std::string> CodeGenerationVisitor::visit(const RangePatternNode &node) {
	// Range pattern start to end or start to= end
	auto startCode = visit(*node.start);
	if (!startCode.has_value()) {
		return Err<std::string>(startCode.error());
	}
	
	auto endCode = visit(*node.end);
	if (!endCode.has_value()) {
		return Err<std::string>(endCode.error());
	}
	
	std::string code = startCode.value() + (node.isInclusive ? " to= " : " to ") + endCode.value();
	return { code };
}

// Helper method for generating destructuring assignments
Result<std::string> CodeGenerationVisitor::generateDestructuring(const PatternNode* pattern, const std::string& sourceVar) {
	std::string code;
	
	switch(pattern->getNodeType()) {
		case ASTNodeType::ArrayPattern: {
			const ArrayPatternNode* arrayPattern = dynamic_cast<const ArrayPatternNode*>(pattern);
			if(!arrayPattern) {
				return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Failed to cast to ArrayPatternNode", pattern->position));
			}
			
			// Generate assignments for each element using runtime utilities
			for(size_t i = 0; i < arrayPattern->elements.size(); ++i) {
				const PatternNode* element = arrayPattern->elements[i].get();
				
				if(element->getNodeType() == ASTNodeType::IdentifierPattern) {
					const IdentifierPatternNode* identifierPattern = dynamic_cast<const IdentifierPatternNode*>(element);
					if(identifierPattern) {
						code += "auto " + identifierPattern->name + " = ArgonLang::Runtime::destructure_array_element(" + sourceVar + ", " + std::to_string(i) + ");";
					}
				} else {
					// Nested destructuring - create a unique temporary variable for the element
					std::string elementVar = "__destructure_elem_" + std::to_string(element->position.line) + "_" + std::to_string(element->position.column) + "_" + std::to_string(i);
					code += "auto " + elementVar + " = ArgonLang::Runtime::destructure_array_element(" + sourceVar + ", " + std::to_string(i) + ");";
					
					Result<std::string> nestedDestructuring = generateDestructuring(element, elementVar);
					if (!nestedDestructuring.has_value()) {
						return Err<std::string>(nestedDestructuring.error());
					}
					code += nestedDestructuring.value();
				}
			}
			
			// Handle rest pattern if present using runtime utilities
			if(arrayPattern->rest) {
				if(arrayPattern->rest->getNodeType() == ASTNodeType::IdentifierPattern) {
					const IdentifierPatternNode* restPattern = dynamic_cast<const IdentifierPatternNode*>(arrayPattern->rest.get());
					if(restPattern) {
						// Generate code to slice the remaining elements using runtime utility
						code += "auto " + restPattern->name + " = ArgonLang::Runtime::destructure_array_rest(" + 
								sourceVar + ", " + std::to_string(arrayPattern->elements.size()) + ");";
					}
				}
			}
			break;
		}
		
		case ASTNodeType::StructPattern: {
			const StructPatternNode* structPattern = dynamic_cast<const StructPatternNode*>(pattern);
			if(!structPattern) {
				return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Failed to cast to StructPatternNode", pattern->position));
			}
			
			// Generate assignments for each field: auto field = sourceVar.fieldName;
			for(const auto& field : structPattern->fields) {
				const std::string& fieldName = field.first;
				const PatternNode* fieldPattern = field.second.get();
				
				if(fieldPattern->getNodeType() == ASTNodeType::IdentifierPattern) {
					const IdentifierPatternNode* identifierPattern = dynamic_cast<const IdentifierPatternNode*>(fieldPattern);
					if(identifierPattern) {
						code += "auto " + identifierPattern->name + " = " + sourceVar + "." + fieldName + ";";
					}
				} else {
					// Nested destructuring - create a unique temporary variable for the field
					std::string fieldVar = "__destructure_field_" + std::to_string(fieldPattern->position.line) + "_" + std::to_string(fieldPattern->position.column) + "_" + fieldName;
					code += "auto " + fieldVar + " = " + sourceVar + "." + fieldName + ";";
					
					Result<std::string> nestedDestructuring = generateDestructuring(fieldPattern, fieldVar);
					if (!nestedDestructuring.has_value()) {
						return Err<std::string>(nestedDestructuring.error());
					}
					code += nestedDestructuring.value();
				}
			}
			break;
		}
		
		case ASTNodeType::IdentifierPattern: {
			const IdentifierPatternNode* identifierPattern = dynamic_cast<const IdentifierPatternNode*>(pattern);
			if(identifierPattern) {
				code += "auto " + identifierPattern->name + " = " + sourceVar + ";";
			}
			break;
		}
		
		default:
			return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Unsupported pattern type for destructuring", pattern->position));
	}
	
	return { code };
}

// Helper method for generating compound destructuring assignments
Result<std::string> CodeGenerationVisitor::generateCompoundDestructuring(const std::vector<std::unique_ptr<PatternNode>>& patterns, const std::string& sourceVar) {
	std::string code;
	
	// Analyze the patterns to determine the destructuring strategy
	int arrayPatternIndex = -1;
	std::vector<int> identifierIndices;
	
	for(size_t i = 0; i < patterns.size(); ++i) {
		if(patterns[i]->getNodeType() == ASTNodeType::ArrayPattern) {
			if(arrayPatternIndex != -1) {
				return Err<std::string>(create_parse_error(ErrorType::InvalidStatement, "Only one array pattern allowed in compound destructuring", patterns[i]->position));
			}
			arrayPatternIndex = static_cast<int>(i);
		} else if(patterns[i]->getNodeType() == ASTNodeType::IdentifierPattern) {
			identifierIndices.push_back(static_cast<int>(i));
		} else {
			return Err<std::string>(create_parse_error(ErrorType::InvalidStatement, "Compound destructuring only supports array patterns and identifier patterns", patterns[i]->position));
		}
	}
	
	if(arrayPatternIndex == -1) {
		return Err<std::string>(create_parse_error(ErrorType::InvalidStatement, "Compound destructuring requires at least one array pattern", patterns[0]->position));
	}
	
	const ArrayPatternNode* arrayPattern = dynamic_cast<const ArrayPatternNode*>(patterns[arrayPatternIndex].get());
	if(!arrayPattern) {
		return Err<std::string>(create_parse_error(ErrorType::InvalidCodeGeneration, "Failed to cast to ArrayPatternNode", patterns[arrayPatternIndex]->position));
	}
	
	size_t arrayElementCount = arrayPattern->elements.size();
	
	// Generate destructuring based on position of array pattern
	if(arrayPatternIndex == 0) {
		// Pattern: [first, second], rest = arr
		// Extract array elements first
		for(size_t i = 0; i < arrayElementCount; ++i) {
			const PatternNode* element = arrayPattern->elements[i].get();
			if(element->getNodeType() == ASTNodeType::IdentifierPattern) {
				const IdentifierPatternNode* identifierPattern = dynamic_cast<const IdentifierPatternNode*>(element);
				if(identifierPattern) {
					code += "auto " + identifierPattern->name + " = " + sourceVar + "[" + std::to_string(i) + "];";
				}
			}
		}
		
		// Generate rest variables (everything after the array elements)
		for(int restIndex : identifierIndices) {
			const IdentifierPatternNode* restPattern = dynamic_cast<const IdentifierPatternNode*>(patterns[restIndex].get());
			if(restPattern) {
				code += "auto " + restPattern->name + " = std::vector<decltype(" + sourceVar + "[0])>(" + 
						sourceVar + ".begin() + " + std::to_string(arrayElementCount) + ", " + sourceVar + ".end());";
			}
		}
	} else {
		// Pattern: rest, [last] = arr or rest1, rest2, [last] = arr
		// Generate rest variables first (everything except the last N elements)
		for(int i = 0; i < arrayPatternIndex; ++i) {
			const IdentifierPatternNode* restPattern = dynamic_cast<const IdentifierPatternNode*>(patterns[i].get());
			if(restPattern) {
				if(i == arrayPatternIndex - 1) {
					// Last rest pattern gets everything except the array elements at the end
					code += "auto " + restPattern->name + " = std::vector<decltype(" + sourceVar + "[0])>(" + 
							sourceVar + ".begin(), " + sourceVar + ".end() - " + std::to_string(arrayElementCount) + ");";
				} else {
					// For multiple rest patterns, we'd need more complex logic
					// For now, just assign empty vectors to intermediate rest patterns
					code += "auto " + restPattern->name + " = std::vector<decltype(" + sourceVar + "[0])>();";
				}
			}
		}
		
		// Extract array elements from the end
		for(size_t i = 0; i < arrayElementCount; ++i) {
			const PatternNode* element = arrayPattern->elements[i].get();
			if(element->getNodeType() == ASTNodeType::IdentifierPattern) {
				const IdentifierPatternNode* identifierPattern = dynamic_cast<const IdentifierPatternNode*>(element);
				if(identifierPattern) {
					// Access from the end: arr[arr.size() - arrayElementCount + i]
					code += "auto " + identifierPattern->name + " = " + sourceVar + "[" + sourceVar + ".size() - " + 
							std::to_string(arrayElementCount) + " + " + std::to_string(i) + "];";
				}
			}
		}
	}
	
	return { code };
}

// Helper method to generate a single generic parameter
Result<std::string> CodeGenerationVisitor::generateGenericParameter(const GenericParameter& param) {
	// Just generate the typename, constraints will be added as requires clause
	return { "typename " + param.name };
}

// Helper method to generate template parameters
Result<std::string> CodeGenerationVisitor::generateGenericParameters(const std::vector<std::unique_ptr<GenericParameter>>& genericParams) {
	if (genericParams.empty()) {
		return { "" };
	}
	
	std::string code = "template<";
	std::vector<std::string> requires_clauses;
	
	for (size_t i = 0; i < genericParams.size(); ++i) {
		if (i > 0) code += ", ";
		
		auto paramResult = generateGenericParameter(*genericParams[i]);
		if (!paramResult.has_value()) {
			return Err<std::string>(paramResult.error());
		}
		
		code += paramResult.value();
		
		// Collect requires clauses for constraints
		if (genericParams[i]->constraint) {
			auto constraintResult = visit(*genericParams[i]->constraint);
			if (!constraintResult.has_value()) {
				return Err<std::string>(constraintResult.error());
			}
			
			std::string constraint = constraintResult.value();
			std::string paramName = genericParams[i]->name;
			
			// Check if this looks like a concept (starts with uppercase) or a concrete type
			if (!constraint.empty() && std::isupper(constraint[0])) {
				// Assume it's a concept, use concept<Type> syntax
				requires_clauses.push_back(constraint + "<" + paramName + ">");
			} else {
				// Assume it's a concrete type, use std::same_as<Type, ConcreteType>
				requires_clauses.push_back("std::same_as<" + paramName + ", " + constraint + ">");
			}
		}
	}
	code += ">";
	
	// Add requires clause if we have constraints
	if (!requires_clauses.empty()) {
		code += " requires ";
		for (size_t i = 0; i < requires_clauses.size(); ++i) {
			if (i > 0) code += " && ";
			code += requires_clauses[i];
		}
	}
	
	code += "\n";
	
	return { code };
}

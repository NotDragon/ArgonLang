#include "backend/AST.h"

#ifdef DEBUG

void ArgonLang::StringLiteralNode::print() const {
	std::cout << "StringLiteral(" << value << ")";
}

void ArgonLang::CharLiteralNode::print() const {
	std::cout << "CharLiteral(" << value << ")";
}

void ArgonLang::IntegralLiteralNode::print() const {
	std::cout << "IntegralLiteral: ";
	switch (type) {
	case PrimitiveType::INT8:
		std::cout << value.i8;
		break;
	case PrimitiveType::INT16:
		std::cout << value.i16;
		break;
	case PrimitiveType::INT32:
		std::cout << value.i32;
		break;
	case PrimitiveType::INT64:
		std::cout << value.i64;
		break;
	case PrimitiveType::INT128:
		std::cout << static_cast<int64_t>(value.i128);
		break;
	default:
		throw std::runtime_error("Invalid type for IntegralLiteralNode");
	}
	std::cout << " (" << primitive_type_to_string(type) << ")\n";
}

void ArgonLang::FloatLiteralNode::print() const {
	std::cout << "FloatLiteral: ";
	switch (type) {
	case PrimitiveType::FLOAT32:
		std::cout << value.f32;
		break;
	case PrimitiveType::FLOAT64:
		std::cout << value.f64;
		break;
	case PrimitiveType::FLOAT128:
		std::cout << value.f128;
		break;
	default:
		throw std::runtime_error("Invalid type for FloatLiteralNode");
	}
	std::cout << " (" << primitive_type_to_string(type) << ")\n";
}

void ArgonLang::BooleanLiteralNode::print() const {
	std::cout << "BooleanLiteral(" << value << ")";
}

void ArgonLang::IdentifierNode::print() const {
	std::cout << "Identifier(" << identifier << ")";
}

void ArgonLang::BinaryExpressionNode::print() const {
	std::cout << "BinaryOp(";
	left->print();
	std::cout << " " << op.type << " ";
	right->print();
	std::cout << ")";
}

void ArgonLang::UnaryExpressionNode::print() const {
	std::cout << "UnaryOp(" << op.value << " ";
	operand->print();
	std::cout << ")";
}

void ArgonLang::ProgramNode::print() const {
	for (const auto& i : nodes) {
		i->print();
	}
}

void ArgonLang::NullExpressionNode::print() const {
	std::cout << "null";
}

void ArgonLang::ToExpressionNode::print() const {
	std::cout << "ToExpression( ";
	if (lowerBound)
		lowerBound->print();
	else
		std::cout << "*";
	std::cout << " to ";
	if (upperBound)
		upperBound->print();
	else
		std::cout << "*";
	std::cout << ")" << std::endl;
}

void ArgonLang::FunctionCallExpressionNode::print() const {
	std::cout << "FunctionCallExpressionNode: ";
	function->print();
	if (!genericTypeArgs.empty()) {
		std::cout << " with generic args: <";
		for (size_t i = 0; i < genericTypeArgs.size(); ++i) {
			if (i > 0)
				std::cout << ", ";
			genericTypeArgs[i]->print();
		}
		std::cout << ">";
	}
	std::cout << " with " << arguments.size() << " arguments\n";
}

void ArgonLang::LambdaExpressionNode::print() const {}

void ArgonLang::ComparisonExpressionNode::print() const {}

void ArgonLang::AssignmentExpressionNode::print() const {}

void ArgonLang::IndexExpressionNode::print() const {}

void ArgonLang::SliceExpressionNode::print() const {}

void ArgonLang::MultipleIndexExpressionNode::print() const {}

void ArgonLang::MatchBranch::print() const {
	std::cout << "Pattern: ";
	if (pattern)
		pattern->print();
	if (condition) {
		std::cout << " [Condition: ";
		condition->print();
		std::cout << "]";
	}
	std::cout << " -> ";
	if (body)
		body->print();
	std::cout << std::endl;
}

void ArgonLang::MatchExpressionNode::print() const {
	std::cout << "Match Expression: \n";
	std::cout << "Value: ";
	if (value)
		value->print();
	std::cout << "\nBranches:\n";
	for (const auto& branch : branches) {
		branch->print();
	}
}

void ArgonLang::TernaryExpressionNode::print() const {
	std::cout << "Conditional Expression:\n";
	std::cout << "Condition: ";
	condition->print();
	std::cout << "True Branch: ";
	trueBranch->print();
	std::cout << "False Branch: ";
	falseBranch->print();
}

void ArgonLang::ReturnStatementNode::print() const {}

void ArgonLang::IntersectionTypeNode::print() const {}

void ArgonLang::PrefixedTypeNode::print() const {}

void ArgonLang::VariableDeclarationNode::print() const {}

void ArgonLang::TypeAliasNode::print() const {
	std::cout << "Type Alias: " << aliasName << " = ";
	if (targetType)
		targetType->print();
	std::cout << std::endl;
}

void ArgonLang::IfStatementNode::print() const {
	std::cout << "If Statement:\n";
	std::cout << "  Condition: ";
	if (condition)
		condition->print();
	std::cout << "\n  Then Branch: ";
	if (body)
		body->print();
	if (elseBranch) {
		std::cout << "\n  Else Branch: ";
		elseBranch->print();
	}
	std::cout << std::endl;
}

void ArgonLang::BlockNode::print() const {}

void ArgonLang::ForStatementNode::print() const {
	std::cout << "For Loop:\n";
	std::cout << "  Variable: " << variableName << "\n";
	std::cout << "  Iterator: ";
	if (iterator)
		iterator->print();
	std::cout << "\n  Body: ";
	if (body)
		body->print();
	std::cout << std::endl;
}

void ArgonLang::UnionDeclarationNode::print() const {
	std::cout << "Union: " << unionName << " = ";
	for (const auto& type : types) {
		type->print();
		if (&type != &types.back())
			std::cout << " | ";
	}
	std::cout << std::endl;
}

void ArgonLang::IdentifierTypeNode::print() const {
	std::cout << "PrimitiveType: " << typeName << std::endl;
}

void ArgonLang::GenericTypeNode::print() const {
	std::cout << "GenericType: " << base << "<";
	for (int i = 0; i < params.size(); ++i) {
		params[i]->print();
		if (i < params.size() - 1)
			std::cout << ", ";
	}
	std::cout << ">" << std::endl;
}

void ArgonLang::SumTypeNode::print() const {
	std::cout << "SumType: ";
	for (int i = 0; i < types.size(); ++i) {
		types[i]->print();
		if (i < types.size() - 1)
			std::cout << " | ";
	}
	std::cout << std::endl;
}

void ArgonLang::RangeExpressionNode::print() const {}

void ArgonLang::YieldStatementNode::print() const {}

void ArgonLang::ParallelExpressionNode::print() const {}

void ArgonLang::TryExpressionNode::print() const {}

void ArgonLang::WhileStatementNode::print() const {}

void ArgonLang::BreakStatementNode::print() const {}

void ArgonLang::ContinueStatementNode::print() const {}

void ArgonLang::StructExpressionNode::print() const {}

void ArgonLang::StructField::print() const {}

void ArgonLang::FunctionArgument::print() const {}

void ArgonLang::FunctionDeclarationNode::print() const {}

void ArgonLang::FunctionDefinitionNode::print() const {}

void ArgonLang::ImplStatementNode::print() const {}

void ArgonLang::ConstructorStatementNode::ConstructorArgument::print() const {}

void ArgonLang::ConstructorStatementNode::print() const {}

void ArgonLang::ClassDeclarationNode::ClassMember::ClassMember::print() const {}

void ArgonLang::ClassDeclarationNode::print() const {
	std::cout << "ClassDeclarationNode: " << className;
	if (!genericParams.empty()) {
		std::cout << "<";
		for (size_t i = 0; i < genericParams.size(); ++i) {
			if (i > 0)
				std::cout << ", ";
			genericParams[i]->print();
		}
		std::cout << ">";
	}
	std::cout << "\n";
	for (const auto& member : body) {
		member.print();
	}
}

void ArgonLang::MemberAccessExpressionNode::print() const {
	std::cout << "MemberAccessExpressionNode: " << member << std::endl;
}

// New AST node print implementations
void ArgonLang::EnumDeclarationNode::print() const {
	std::cout << "EnumDeclarationNode: " << enumName << " (isUnion: " << isUnion << ")\n";
	for (const auto& variant : variants) {
		std::cout << "  Variant: " << variant.name << "\n";
	}
}

void ArgonLang::ConstraintDeclarationNode::print() const {
	std::cout << "ConstraintDeclarationNode: " << constraintName << "\n";
}

void ArgonLang::GenericParameter::print() const {
	std::cout << "GenericParameterNode: " << name;
	if (constraint) {
		std::cout << " : ";
		constraint->print();
	}
	std::cout << "\n";
}

void ArgonLang::ModuleDeclarationNode::print() const {
	std::cout << "ModuleDeclarationNode: " << moduleName << "\n";
}

void ArgonLang::ImportStatementNode::print() const {
	std::cout << "ImportStatementNode: " << moduleName << "\n";
}

void ArgonLang::FunctionTypeNode::print() const {
	std::cout << "FunctionType: ";
	if (isClosure) {
		std::cout << "func ";
		if (returnType)
			returnType->print();
	} else {
		std::cout << "func(";
		for (int i = 0; i < parameterTypes.size(); ++i) {
			parameterTypes[i]->print();
			if (i < parameterTypes.size() - 1)
				std::cout << ", ";
		}
		std::cout << ")";
		if (returnType) {
			std::cout << " ";
			returnType->print();
		}
	}
}

void ArgonLang::ArrayTypeNode::print() const {
	std::cout << "ArrayType: ";
	elementType->print();
	std::cout << "[";
	if (size)
		size->print();
	std::cout << "]";
}

void ArgonLang::VariadicTypeNode::print() const {
	std::cout << "VariadicType: ...";
	baseType->print();
}

// Pattern node print methods
void ArgonLang::WildcardPatternNode::print() const {
	std::cout << "WildcardPattern(_)";
}

void ArgonLang::LiteralPatternNode::print() const {
	std::cout << "LiteralPattern(";
	literal->print();
	std::cout << ")";
}

void ArgonLang::IdentifierPatternNode::print() const {
	std::cout << "IdentifierPattern(" << name << ")";
}

void ArgonLang::ArrayPatternNode::print() const {
	std::cout << "ArrayPattern([";
	for (size_t i = 0; i < elements.size(); ++i) {
		if (i > 0)
			std::cout << ", ";
		elements[i]->print();
	}
	if (rest) {
		std::cout << ", ...";
		rest->print();
	}
	std::cout << "])";
}

void ArgonLang::StructPatternNode::print() const {
	std::cout << "StructPattern({";
	for (size_t i = 0; i < fields.size(); ++i) {
		if (i > 0)
			std::cout << ", ";
		std::cout << fields[i].first << ": ";
		fields[i].second->print();
	}
	std::cout << "})";
}

void ArgonLang::ConstructorPatternNode::print() const {
	std::cout << "ConstructorPattern(" << constructorName << "(";
	for (size_t i = 0; i < arguments.size(); ++i) {
		if (i > 0)
			std::cout << ", ";
		arguments[i]->print();
	}
	std::cout << "))";
}

void ArgonLang::TypePatternNode::print() const {
	std::cout << "TypePattern(";
	type->print();
	std::cout << ")";
}

void ArgonLang::RangePatternNode::print() const {
	std::cout << "RangePattern(";
	start->print();
	std::cout << (isInclusive ? " to= " : " to ");
	end->print();
	std::cout << ")";
}

#endif
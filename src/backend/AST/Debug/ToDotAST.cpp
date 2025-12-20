#include "backend/AST.h"
#ifdef DEBUG

void ArgonLang::StringLiteralNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"StringLiteral(" << value << ")\"];\n";
}

void ArgonLang::CharLiteralNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"CharLiteral(" << value << ")\"];\n";
}

void ArgonLang::IntegralLiteralNode::to_dot(std::ostream& os, int& nodeId) const {
	os << "  node" << nodeId++ << " [label=\"IntegralLiteral: " << value 
	   << " (" << primitive_type_to_string(type) << ")\"];\n";
}

void ArgonLang::FloatLiteralNode::to_dot(std::ostream& os, int& nodeId) const {
	os << "  node" << nodeId++ << " [label=\"FloatLiteral: " << value 
	   << " (" << primitive_type_to_string(type) << ")\"];\n";
}

void ArgonLang::IdentifierNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Identifier(" << identifier << ")\"];\n";
}

void ArgonLang::UnaryExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"UnaryExpressionNode(" << op.value << ")\"];\n";
	if (operand) {
		int leftId = nodeId;
		operand->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << ";\n";
	}
}

void ArgonLang::UnaryPostExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"UnaryPostExpressionNode(" << op.value << ")\"];\n";
	if (operand) {
		int leftId = nodeId;
		operand->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << ";\n";
	}
}

void ArgonLang::BooleanLiteralNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"BooleanLiteral(" << value << ")\"];\n";
}

void ArgonLang::BinaryExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"BinaryExpression(" << op.value << ")\"];\n";

	if (left) {
		int leftId = nodeId;
		left->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << ";\n";
	}
	if (right) {
		int rightId = nodeId;
		right->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << rightId << ";\n";
	}
}

void ArgonLang::ProgramNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Program\"];\n";

	if (nodes.empty())
		return;

	for (const auto& statement : nodes) {
		int branchId = nodeId;
		statement->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << branchId << " [label=\"node\"];\n";
	}
}

void ArgonLang::NullExpressionNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ToExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Range\"];\n";

	if (lowerBound) {
		int lowerId = nodeId;
		lowerBound->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << lowerId << " [label=\"lower\"];\n";
	} else {
		os << "  node" << currentId << " -> node" << nodeId++ << " [label=\"lower: *\"];\n";
	}

	if (upperBound) {
		int upperId = nodeId;
		upperBound->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << upperId << " [label=\"upper\"];\n";
	} else {
		os << "  node" << currentId << " -> node" << nodeId++ << " [label=\"upper: *\"];\n";
	}
}

void ArgonLang::FunctionCallExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	std::string label = "FunctionCallExpression";
	if (!genericTypeArgs.empty()) {
		label += "<" + std::to_string(genericTypeArgs.size()) + " types>";
	}
	os << "  node" << currentId << " [label=\"" << label << "\"];\n";

	int nameId = nodeId;
	function->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << nameId << " [label=\"Name\"];\n";

	for (size_t i = 0; i < genericTypeArgs.size(); ++i) {
		int typeId = nodeId;
		genericTypeArgs[i]->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << typeId << " [label=\"TypeArg" << i << "\"];\n";
	}

	for (const auto& argument : arguments) {
		int argumentId = nodeId;
		argument->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << argumentId << " [label=\"Argument\"];\n";
	}
}

void ArgonLang::LambdaExpressionNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ComparisonExpressionNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::IndexExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;

	os << "  node" << currentId << " [label=\"IndexExpression\"];\n";

	if (array) {
		int arrayId = nodeId;
		array->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << arrayId << " [label=\"array\"];\n";
	}

	if (index) {
		int indexId = nodeId;
		index->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << indexId << " [label=\"index\"];\n";
	}
}

void ArgonLang::SliceExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;

	os << "  node" << currentId << " [label=\"SliceExpression\"];\n";

	if (array) {
		int arrayId = nodeId;
		array->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << arrayId << " [label=\"array\"];\n";
	}

	if (start) {
		int startId = nodeId;
		start->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << startId << " [label=\"start\"];\n";
	}

	if (end) {
		int endId = nodeId;
		end->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << endId << " [label=\"end\"];\n";
	}
}

void ArgonLang::MultipleIndexExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;

	os << "  node" << currentId << " [label=\"MultipleIndexExpression\"];\n";

	if (array) {
		int arrayId = nodeId;
		array->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << arrayId << " [label=\"array\"];\n";
	}

	for (size_t i = 0; i < indices.size(); ++i) {
		if (indices[i]) {
			int indexId = nodeId;
			indices[i]->to_dot(os, nodeId);
			os << "  node" << currentId << " -> node" << indexId << " [label=\"index" << i << "\"];\n";
		}
	}
}

void ArgonLang::MatchBranch::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Match Branch\"];\n";

	if (pattern) {
		int patternId = nodeId;
		pattern->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << patternId << " [label=\"pattern\"];\n";
	}
	if (condition) {
		int conditionId = nodeId;
		condition->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}
	if (body) {
		int bodyId = nodeId;
		body->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << bodyId << " [label=\"body\"];\n";
	}
}

void ArgonLang::MatchExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Match\"];\n";

	if (value) {
		int valueId = nodeId;
		value->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << valueId << " [label=\"value\"];\n";
	}

	for (const auto& branch : branches) {
		int branchId = nodeId;
		branch->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << branchId << " [label=\"branch\"];\n";
	}
}

void ArgonLang::TernaryExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Conditional\"];\n";

	if (condition) {
		int conditionId = nodeId;
		condition->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}

	if (trueBranch) {
		int trueId = nodeId;
		trueBranch->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << trueId << " [label=\"true\"];\n";
	}

	if (falseBranch) {
		int falseId = nodeId;
		falseBranch->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << falseId << " [label=\"false\"];\n";
	}
}

void ArgonLang::ReturnStatementNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ParallelExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"ParallelExpression\"];\n";
	statementNode->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
}

void ArgonLang::TryExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"TryExpression\"];\n";
	expression->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
}

void ArgonLang::StructExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"StructExpressionNode\"];\n";
	for (const auto& filed : fields) {
		int filedId = nodeId++;
		filed.toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << filedId << ";\n";
	}
}

void ArgonLang::RangeExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentNodeId = nodeId++;
	os << "  node" << currentNodeId << " [label=\"RangeExpression\"];\n";

	for (const auto& element : range) {
		int elementNodeId = nodeId;
		element->to_dot(os, nodeId);
		os << "  node" << currentNodeId << " -> node" << elementNodeId << ";\n";
	}
}

void ArgonLang::VariableDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"VariableDeclarationNode(" << (isConst ? "const" : "") << "): " << name
	   << "\"];\n";

	if (type) {
		int targetId = nodeId;
		type->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << targetId << " [label=\"type\"];\n";
	}

	if (value) {
		int targetId = nodeId;
		value->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << targetId << " [label=\"value\"];\n";
	}
}

void ArgonLang::TypeAliasNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"TypeAlias: " << aliasName << "\"];\n";

	if (targetType) {
		int targetId = nodeId;
		targetType->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << targetId << " [label=\"target\"];\n";
	}
}

void ArgonLang::IfStatementNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"If\"];\n";

	if (condition) {
		int conditionId = nodeId;
		condition->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}

	if (body) {
		int thenId = nodeId;
		body->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << thenId << " [label=\"then\"];\n";
	}

	if (elseBranch) {
		int elseId = nodeId;
		elseBranch->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << elseId << " [label=\"else\"];\n";
	}
}

void ArgonLang::ForStatementNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"For\"];\n";

	int varId = nodeId++;
	os << "  node" << varId << " [label=\"Variable: " << variableName << "\"];\n";
	os << "  node" << currentId << " -> node" << varId << " [label=\"variable\"];\n";

	if (iterator) {
		int iterId = nodeId;
		iterator->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << iterId << " [label=\"iterator\"];\n";
	}

	if (body) {
		int bodyId = nodeId;
		body->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << bodyId << " [label=\"body\"];\n";
	}
}

void ArgonLang::UnionDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Union: " << unionName << "\"];\n";

	for (const auto& field : fields) {
		int fieldId = nodeId++;
		os << "  node" << fieldId << " [label=\"Field: " << field.name << "\"];\n";
		os << "  node" << currentId << " -> node" << fieldId << " [label=\"field\"];\n";
		if (field.type) {
			int typeId = nodeId;
			field.type->to_dot(os, nodeId);
			os << "  node" << fieldId << " -> node" << typeId << " [label=\"type\"];\n";
		}
	}
}

void ArgonLang::YieldStatementNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::WhileStatementNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"" << (isDoWhile ? "Dowhile" : "While") << "\"];\n";

	if (condition) {
		int conditionId = nodeId;
		condition->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}

	if (body) {
		int thenId = nodeId;
		body->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << thenId << " [label=\"then\"];\n";
	}

	if (elseBranch) {
		int elseId = nodeId;
		elseBranch->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << elseId << " [label=\"else\"];\n";
	}
}

void ArgonLang::BreakStatementNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"BreakStatementNode\"];\n";
}

void ArgonLang::ContinueStatementNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"ContinueStatementNode\"];\n";
}

void ArgonLang::BlockNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Block\"];\n";
	if (body.empty())
		return;
	for (const auto& node : body) {
		int branchId = nodeId;
		node->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << branchId << ";\n";
	}
}

void ArgonLang::StructField::toDot(std::ostream& os, int& nodeId) const {
	int filedId = nodeId++;
	os << " node" << filedId << " [label=\"Name: " << name << "\"];\n";

	if (type) {
		int typeId = nodeId;
		type->to_dot(os, nodeId);
		os << "  node" << filedId << " -> node" << typeId << " [label=\"Type\"];\n";
	}

	if (value) {
		int expressionId = nodeId;
		value->to_dot(os, nodeId);
		os << "  node" << filedId << " -> node" << expressionId << " [label=\"Value\"];\n";
	}
}

void ArgonLang::FunctionArgument::toDot(std::ostream& os, int& nodeId) const {
	int filedId = nodeId++;
	os << " node" << filedId << " [label=\"FunctionArgument: " << name << "\"];\n";

	if (!type)
		return;
	int typeId = nodeId;
	type->to_dot(os, nodeId);
	os << "  node" << filedId << " -> node" << typeId << " [label=\"Type\"];\n";

	if (!value)
		return;
	int expressionId = nodeId;
	value->to_dot(os, nodeId);
	os << "  node" << filedId << " -> node" << expressionId << " [label=\"Value\"];\n";
}

void ArgonLang::FunctionDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"" << "Function Declaration\"];\n";

	int nameId = nodeId;
	name->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << nameId << " [label=\"Name\"];\n";

	for (const auto& arg : args) {
		int argId = nodeId;
		arg->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << argId << " [label=\"arg\"];\n";
	}

	int thenId = nodeId;
	body->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << thenId << " [label=\"body\"];\n";

	if (!returnType)
		return;

	int typeId = nodeId;
	returnType->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];\n";
}

void ArgonLang::FunctionDefinitionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"" << "Function Definition: \"];\n";

	int nameId = nodeId;
	name->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << nameId << " [label=\"Name\"];\n";

	for (const auto& arg : args) {
		int argId = nodeId;
		arg->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << argId << " [label=\"arg\"];\n";
	}

	if (!returnType)
		return;

	int typeId = nodeId;
	returnType->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];\n";
}

void ArgonLang::ImplStatementNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ConstructorStatementNode::ConstructorArgument::toDot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ConstructorStatementNode::to_dot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ClassDeclarationNode::ClassMember::toDot(std::ostream& os, int& nodeId) const {}

void ArgonLang::ClassDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	std::string label = "Class: " + className;
	if (!genericParams.empty()) {
		label += "<";
		for (size_t i = 0; i < genericParams.size(); ++i) {
			if (i > 0)
				label += ", ";
			label += genericParams[i]->name;
		}
		label += ">";
	}
	os << "  node" << currentId << " [label=\"" << label << "\"];\n";

	for (const auto& member : body) {
		member.toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << (nodeId - 1) << ";\n";
	}
}

void ArgonLang::MemberAccessExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Member Access\"];\n";

	if (parent) {
		int leftId = nodeId;
		parent->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << ";\n";
	}

	if (member) {
		int memberId = nodeId;
		member->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << memberId << ";\n";
	}
}

void ArgonLang::AssignmentExpressionNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Assignment\"];\n";

	if (left) {
		int leftId = nodeId;
		left->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << " [label=\"parent\"];\n";
	}

	if (right) {
		int rightId = nodeId;
		right->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << rightId << " [label=\"right\"];\n";
	}
}

void ArgonLang::IdentifierTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"IdentifierType: " << typeName << "\"];";
}

void ArgonLang::GenericTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"GenericType\"];";

	int baseId = nodeId;
	base->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << baseId << " [label=\"base\"];";

	for (const auto& param : params) {
		int paramId = nodeId;
		param->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << paramId << " [label=\"param\"];";
	}
}

void ArgonLang::SumTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"SumType\"];";

	for (const auto& type : types) {
		int typeId = nodeId;
		type->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];";
	}
}

void ArgonLang::IntersectionTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"IntersectionType\"];";

	for (const auto& type : types) {
		int typeId = nodeId;
		type->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];";
	}
}

void ArgonLang::PrefixedTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	std::string prefixLabel = (prefix == Prefix::Pointer) ? "Pointer" : "Owned";
	os << "  node" << currentId << " [label=\"PrefixedType: " << prefixLabel << "\"];";

	int typeId = nodeId;
	type->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];";
}

// New AST node toDot implementations
void ArgonLang::EnumDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Enum: " << enumName << " (isUnion: " << isUnion << ")\"];\n";

	if (constraintType) {
		int constraintId = nodeId;
		constraintType->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << constraintId << " [label=\"constraint\"];\n";
	}

	for (const auto& variant : variants) {
		int variantId = nodeId++;
		os << "  node" << variantId << " [label=\"Variant: " << variant.name << "\"];\n";
		os << "  node" << currentId << " -> node" << variantId << " [label=\"variant\"];\n";

		if (variant.explicitValue) {
			int valueId = nodeId;
			variant.explicitValue->to_dot(os, nodeId);
			os << "  node" << variantId << " -> node" << valueId << " [label=\"value\"];\n";
		}

		for (const auto& field : variant.fields) {
			int fieldId = nodeId++;
			os << "  node" << fieldId << " [label=\"Field: " << field.name << "\"];\n";
			os << "  node" << variantId << " -> node" << fieldId << " [label=\"field\"];\n";
			if (field.type) {
				int typeId = nodeId;
				field.type->to_dot(os, nodeId);
				os << "  node" << fieldId << " -> node" << typeId << " [label=\"type\"];\n";
			}
		}
	}
}

void ArgonLang::ConstraintDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Constraint: " << constraintName << "\"];\n";
}

void ArgonLang::GenericParameter::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	std::string label = "GenericParam: " + name;
	if (constraint) {
		// For simplicity, just show that there's a constraint
		label += " : <constraint>";
	}
	os << "  node" << currentId << " [label=\"" << label << "\"];\n";
}

void ArgonLang::ModuleDeclarationNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Module: " << moduleName << "\"];\n";
}

void ArgonLang::ImportStatementNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Import: " << moduleName << "\"];\n";
}

void ArgonLang::FunctionTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	std::string label = isClosure ? "FunctionType (closure)" : "FunctionType";
	os << "  node" << currentId << " [label=\"" << label << "\"];\n";

	for (const auto& paramType : parameterTypes) {
		int paramId = nodeId;
		paramType->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << paramId << " [label=\"param\"];\n";
	}

	if (returnType) {
		int retId = nodeId;
		returnType->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << retId << " [label=\"return\"];\n";
	}
}

void ArgonLang::ArrayTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"ArrayType\"];\n";

	int elemId = nodeId;
	elementType->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << elemId << " [label=\"element\"];\n";

	if (size) {
		int sizeId = nodeId;
		size->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << sizeId << " [label=\"size\"];\n";
	}
}

void ArgonLang::VariadicTypeNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"VariadicType\"];\n";

	int baseId = nodeId;
	baseType->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << baseId << " [label=\"base\"];\n";
}

// Pattern node toDot methods
void ArgonLang::WildcardPatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"_\"];\n";
}

void ArgonLang::LiteralPatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"LiteralPattern\"];\n";

	int literalId = nodeId;
	literal->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << literalId << " [label=\"literal\"];\n";
}

void ArgonLang::IdentifierPatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"" << name << "\"];\n";
}

void ArgonLang::ArrayPatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"ArrayPattern\"];\n";

	for (size_t i = 0; i < elements.size(); ++i) {
		int elemId = nodeId;
		elements[i]->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << elemId << " [label=\"elem" << i << "\"];\n";
	}

	if (rest) {
		int restId = nodeId;
		rest->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << restId << " [label=\"rest\"];\n";
	}
}

void ArgonLang::StructPatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"StructPattern\"];\n";

	for (size_t i = 0; i < fields.size(); ++i) {
		int fieldId = nodeId;
		fields[i].second->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << fieldId << " [label=\"" << fields[i].first << "\"];\n";
	}
}

void ArgonLang::ConstructorPatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"" << constructorName << "\"];\n";

	for (size_t i = 0; i < arguments.size(); ++i) {
		int argId = nodeId;
		arguments[i]->to_dot(os, nodeId);
		os << "  node" << currentId << " -> node" << argId << " [label=\"arg" << i << "\"];\n";
	}
}

void ArgonLang::TypePatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"TypePattern\"];\n";

	int typeId = nodeId;
	type->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];\n";
}

void ArgonLang::RangePatternNode::to_dot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"RangePattern\"];\n";

	int startId = nodeId;
	start->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << startId << " [label=\"start\"];\n";

	int endId = nodeId;
	end->to_dot(os, nodeId);
	os << "  node" << currentId << " -> node" << endId << " [label=\"end\"];\n";
}

#endif
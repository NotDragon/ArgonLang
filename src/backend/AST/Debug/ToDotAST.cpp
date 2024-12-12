#include "backend/AST.h"
#ifdef DEBUG
void ArgonLang::StringLiteralNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"StringLiteral(" << value << ")\"];\n";
}

void ArgonLang::IntegralLiteralNode::toDot(std::ostream& os, int& nodeId) const {
	os << "  node" << nodeId++ << " [label=\"IntegralLiteral: ";
	switch (type) {
		case PrimitiveType::INT8:   os << static_cast<int>(value.i8); break;
		case PrimitiveType::INT16:  os << value.i16; break;
		case PrimitiveType::INT32:  os << value.i32; break;
		case PrimitiveType::INT64:  os << value.i64; break;
		case PrimitiveType::INT128: os << static_cast<int64_t>(value.i128); break;
		default:
			throw std::runtime_error("Invalid type for IntegralLiteralNode");
	}
	os << " (" << primitiveTypeToString(type) << ")\"];\n";
}

void ArgonLang::FloatLiteralNode::toDot(std::ostream& os, int& nodeId) const {
	os << "  node" << nodeId++ << " [label=\"FloatLiteral: ";
	switch (type) {
		case PrimitiveType::FLOAT32:  os << value.f32; break;
		case PrimitiveType::FLOAT64:  os << value.f64; break;
		case PrimitiveType::FLOAT128: os << value.f128; break;
		default:
			throw std::runtime_error("Invalid type for FloatLiteralNode");
	}
	os << " (" << primitiveTypeToString(type) << ")\"];\n";
}

void ArgonLang::IdentifierNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Identifier(" << identifier << ")\"];\n";
}

void ArgonLang::UnaryExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"UnaryExpressionNode(" << op.value << ")\"];\n";
	if (operand) {
		int leftId = nodeId;
		operand->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << ";\n";
	}
}

void ArgonLang::BooleanLiteralNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"BooleanLiteral(" << value << ")\"];\n";
}

void ArgonLang::BinaryExpressionNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"BinaryExpression(" << op.value << ")\"];\n";

	if (left) {
		int leftId = nodeId;
		left->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << leftId << ";\n";
	}
	if (right) {
		int rightId = nodeId;
		right->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << rightId << ";\n";
	}
}

void ArgonLang::ProgramNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Program\"];\n";

	if (nodes.empty()) return;

	for (const auto& statement : nodes) {
		int branchId = nodeId;
		statement->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << branchId << " [label=\"node\"];\n";
	}
}

void ArgonLang::NullExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::ToExpressionNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Range\"];\n";

	if (lowerBound) {
		int lowerId = nodeId;
		lowerBound->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << lowerId << " [label=\"lower\"];\n";
	} else {
		os << "  node" << currentId << " -> node" << nodeId++ << " [label=\"lower: *\"];\n";
	}

	if (upperBound) {
		int upperId = nodeId;
		upperBound->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << upperId << " [label=\"upper\"];\n";
	} else {
		os << "  node" << currentId << " -> node" << nodeId++ << " [label=\"upper: *\"];\n";
	}
}

void ArgonLang::FunctionCallExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::LambdaExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::ComparisonExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::AssignmentExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::IndexExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::MatchBranch::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Match Branch\"];\n";

	if (pattern) {
		int patternId = nodeId;
		pattern->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << patternId << " [label=\"pattern\"];\n";
	}
	if (condition) {
		int conditionId = nodeId;
		condition->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}
	if (body) {
		int bodyId = nodeId;
		body->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << bodyId << " [label=\"body\"];\n";
	}
}

void ArgonLang::MatchExpressionNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Match\"];\n";

	if (value) {
		int valueId = nodeId;
		value->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << valueId << " [label=\"value\"];\n";
	}

	for (const auto& branch : branches) {
		int branchId = nodeId;
		branch->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << branchId << " [label=\"branch\"];\n";
	}
}

void ArgonLang::TernaryExpressionNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Conditional\"];\n";

	if (condition) {
		int conditionId = nodeId;
		condition->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}

	if (trueBranch) {
		int trueId = nodeId;
		trueBranch->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << trueId << " [label=\"true\"];\n";
	}

	if (falseBranch) {
		int falseId = nodeId;
		falseBranch->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << falseId << " [label=\"false\"];\n";
	}
}

void ArgonLang::ReturnStatementNode::toDot(std::ostream &os, int &nodeId) const {
	
}

void ArgonLang::ParallelExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"ParallelExpression\"];\n";
	statementNode->toDot(os, nodeId);
	os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
}

void ArgonLang::AwaitExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"AwaitExpression\"];\n";
	statementNode->toDot(os, nodeId);
	os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
}

void ArgonLang::LazyExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"LazyExpression\"];\n";
	statementNode->toDot(os, nodeId);
	os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
}

void ArgonLang::StructExpressionNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"StructExpressionNode\"];\n";
	for(const auto& [name, typeNode, expressionNode]: fields) {
		int filedId = nodeId++;
		os << " node" << filedId << " [label=\"Name: " << name << "\"];\n";

		int typeId = nodeId;
		typeNode->toDot(os, nodeId);
		os << "  node" << filedId << " -> node" << typeId << " [label=\"Type\"];\n";
		int expressionId = nodeId;
		expressionNode->toDot(os, nodeId);
		os << "  node" << filedId << " -> node" << expressionId << " [label=\"Value\"];\n";

		os << "  node" << currentId << " -> node" << filedId << ";\n";
	}
}

void ArgonLang::RangeExpressionNode::toDot(std::ostream &os, int &nodeId) const {

}

void ArgonLang::VariableDeclarationNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"VariableDeclarationNode(" << (isConst ? "const" : "") << "): " << name << "\"];\n";

	if (type) {
		int targetId = nodeId;
		type->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << targetId << " [label=\"type\"];\n";
	}

	if (value) {
		int targetId = nodeId;
		value->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << targetId << " [label=\"value\"];\n";
	}
}

void ArgonLang::TypeAliasNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"TypeAlias: " << aliasName << "\"];\n";

	if (targetType) {
		int targetId = nodeId;
		targetType->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << targetId << " [label=\"target\"];\n";
	}
}

void ArgonLang::IfStatementNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"If\"];\n";

	if (condition) {
		int conditionId = nodeId;
		condition->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}

	if (body) {
		int thenId = nodeId;
		body->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << thenId << " [label=\"then\"];\n";
	}

	if (elseBranch) {
		int elseId = nodeId;
		elseBranch->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << elseId << " [label=\"else\"];\n";
	}
}

void ArgonLang::ForStatementNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"For\"];\n";

	int varId = nodeId++;
	os << "  node" << varId << " [label=\"Variable: " << variableName << "\"];\n";
	os << "  node" << currentId << " -> node" << varId << " [label=\"variable\"];\n";

	if (iterator) {
		int iterId = nodeId;
		iterator->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << iterId << " [label=\"iterator\"];\n";
	}

	if (body) {
		int bodyId = nodeId;
		body->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << bodyId << " [label=\"body\"];\n";
	}
}

void ArgonLang::UnionDeclarationNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Union: " << unionName << "\"];\n";

	for (const auto& type : types) {
		int typeId = nodeId;
		type->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];\n";
	}
}

void ArgonLang::GenericTypeNode::toDot(std::ostream& os, int& nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"GenericType: " << name << "\"];\n";
	for (const auto &param: params) {
		int paramId = nodeId;
		param->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << paramId << " [label=\"param\"];\n";
	}
}


void ArgonLang::WhenStatementNode::toDot(std::ostream &os, int &nodeId) const {

}

void ArgonLang::YieldStatementNode::toDot(std::ostream &os, int &nodeId) const {

}

void ArgonLang::WhileStatementNode::toDot(std::ostream &os, int &nodeId) const {

	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"" << (isDoWhile? "Dowhile": "While") << "\"];\n";

	if (condition) {
		int conditionId = nodeId;
		condition->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << conditionId << " [label=\"condition\"];\n";
	}

	if (body) {
		int thenId = nodeId;
		body->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << thenId << " [label=\"then\"];\n";
	}

	if (elseBranch) {
		int elseId = nodeId;
		elseBranch->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << elseId << " [label=\"else\"];\n";
	}
}

void ArgonLang::BreakStatementNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"BreakStatementNode\"];\n";
}

void ArgonLang::ContinueStatementNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"ContinueStatementNode\"];\n";
}

void ArgonLang::BlockNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"Block\"];\n";
	if(body.empty()) return;
	for (const auto& node : body) {
		int branchId = nodeId;
		node->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << branchId << ";\n";
	}
}

void ArgonLang::UnionTypeNode::toDot(std::ostream &os, int &nodeId) const {
	int currentId = nodeId++;
	os << "  node" << currentId << " [label=\"UnionType\"];\n";
	for (const auto& type : types) {
		int typeId = nodeId;
		type->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << typeId << " [label=\"type\"];\n";
	}
}

void ArgonLang::IdentifierTypeNode::toDot(std::ostream& os, int& nodeId) const {
	os << "  node" << nodeId++ << " [label=\"PrimitiveType: " << typeName << "\"];\n";
}

#endif
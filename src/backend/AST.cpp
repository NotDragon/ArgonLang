#include "backend/AST.h"
#include <utility>


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

	// Default to decimal32
	return PrimitiveType::FLOAT32;
}

#ifdef DEBUG
    void ArgonLang::StringLiteralNode::print() const {
        std::cout << "StringLiteral(" << value << ")";
    }
	void ArgonLang::IntegralLiteralNode::print() const {
	std::cout << "IntegralLiteral: ";
		switch (type) {
			case PrimitiveType::INT8:   std::cout << static_cast<int>(value.i8); break;
			case PrimitiveType::INT16:  std::cout << value.i16; break;
			case PrimitiveType::INT32:  std::cout << value.i32; break;
			case PrimitiveType::INT64:  std::cout << value.i64; break;
			case PrimitiveType::INT128: std::cout << static_cast<int64_t>(value.i128); break;
			default:
				throw std::runtime_error("Invalid type for IntegralLiteralNode");
		}
		std::cout << " (" << primitiveTypeToString(type) << ")\n";
	}
	void ArgonLang::FloatLiteralNode::print() const {
		std::cout << "FloatLiteral: ";
		switch (type) {
			case PrimitiveType::FLOAT32:  std::cout << value.f32; break;
			case PrimitiveType::FLOAT64:  std::cout << value.f64; break;
			case PrimitiveType::FLOAT128: std::cout << value.f128; break;
			default:
				throw std::runtime_error("Invalid type for FloatLiteralNode");
		}
		std::cout << " (" << primitiveTypeToString(type) << ")\n";
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
		for(const auto& i: nodes) {
			i->print();
		}
	}

	void ArgonLang::NullExpressionNode::print() const {
		std::cout << "null";
	}

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
        if(operand) {
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

		if(nodes.empty()) return;

		for (const auto& statement : nodes) {
			int branchId = nodeId;
			statement->toDot(os, nodeId);
			os << "  node" << currentId << " -> node" << branchId << " [label=\"node\"];\n";
		}
	}

	void ArgonLang::NullExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}

	void ArgonLang::ToExpressionNode::print() const {
		std::cout << "ToExpression( ";
		if (lowerBound) lowerBound->print();
		else std::cout << "*";
		std::cout << " to ";
		if (upperBound) upperBound->print();
		else std::cout << "*";
		std::cout << ")" << std::endl;
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

	void ArgonLang::FunctionCallExpressionNode::print() const {

	}

	void ArgonLang::FunctionCallExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}

	void ArgonLang::LambdaExpressionNode::print() const {

	}

	void ArgonLang::LambdaExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}
	void ArgonLang::ComparisonExpressionNode::print() const {

	}

	void ArgonLang::ComparisonExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}
	void ArgonLang::AssignmentExpressionNode::print() const {

	}

	void ArgonLang::AssignmentExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}
	void ArgonLang::IndexExpressionNode::print() const {

	}

	void ArgonLang::IndexExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}

	void ArgonLang::MatchBranch::print() const {
		std::cout << "Pattern: ";
		if (pattern) pattern->print();
		if (condition) {
			std::cout << " [Condition: ";
			condition->print();
			std::cout << "]";
		}
		std::cout << " -> ";
		if (body) body->print();
		std::cout << std::endl;
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

	void ArgonLang::MatchExpressionNode::print() const {
		std::cout << "Match Expression: \n";
		std::cout << "Value: ";
		if (value) value->print();
		std::cout << "\nBranches:\n";
		for (const auto& branch : branches) {
			branch->print();
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

	void ArgonLang::TernaryExpressionNode::print() const {
		std::cout << "Conditional Expression:\n";
		std::cout << "Condition: ";
		condition->print();
		std::cout << "True Branch: ";
		trueBranch->print();
		std::cout << "False Branch: ";
		falseBranch->print();
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

	void ArgonLang::ReturnStatementNode::print() const {

	}

	void ArgonLang::ReturnStatementNode::toDot(std::ostream &os, int &nodeId) const {

	}

	void ArgonLang::VariableDeclarationNode::print() const {

	}

	void ArgonLang::VariableDeclarationNode::toDot(std::ostream &os, int &nodeId) const {
		int currentId = nodeId++;
		os << "  node" << currentId << " [label=\"VariableDeclarationNode(" << (isConst? "const": "") << "): " << name << "\"];\n";

		if (type) {
			int targetId = nodeId;
			type->toDot(os, nodeId);
			os << "  node" << currentId << " -> node" << targetId << " [label=\"type\"];\n";
		}

		if(value) {
			int targetId = nodeId;
			value->toDot(os, nodeId);
			os << "  node" << currentId << " -> node" << targetId << " [label=\"value\"];\n";
		}
	}

	void ArgonLang::TypeAliasNode::print() const {
		std::cout << "Type Alias: " << aliasName << " = ";
		if (targetType) targetType->print();
		std::cout << std::endl;
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

	void ArgonLang::IfStatementNode::print() const {
		std::cout << "If Statement:\n";
		std::cout << "  Condition: ";
		if (condition) condition->print();
		std::cout << "\n  Then Branch: ";
		if (body) body->print();
		if (elseBranch) {
			std::cout << "\n  Else Branch: ";
			elseBranch->print();
		}
		std::cout << std::endl;
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

	void ArgonLang::BlockNode::print() const {

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

	void ArgonLang::ForStatementNode::print() const {
		std::cout << "For Loop:\n";
		std::cout << "  Variable: " << variableName << "\n";
		std::cout << "  Iterator: ";
		if (iterator) iterator->print();
		std::cout << "\n  Body: ";
		if (body) body->print();
		std::cout << std::endl;
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

	void ArgonLang::UnionDeclarationNode::print() const {
		std::cout << "Union: " << unionName << " = ";
		for (const auto& type : types) {
			type->print();
			if (&type != &types.back()) std::cout << " | ";
		}
		std::cout << std::endl;
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

	void ArgonLang::IdentifierTypeNode::print() const {
		std::cout << "PrimitiveType: " <<  typeName << std::endl;
	}

	void ArgonLang::IdentifierTypeNode::toDot(std::ostream& os, int& nodeId) const {
		os << "  node" << nodeId++ << " [label=\"PrimitiveType: " << typeName << "\"];\n";
	}

	void ArgonLang::GenericTypeNode::print() const {
		std::cout << "GenericType: " << name << "<";
		for (size_t i = 0; i < params.size(); ++i) {
			params[i]->print();
			if (i < params.size() - 1) std::cout << ", ";
		}
		std::cout << ">" << std::endl;
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

	void ArgonLang::UnionTypeNode::print() const {
		std::cout << "UnionType: ";
		for (size_t i = 0; i < types.size(); ++i) {
			types[i]->print();
			if (i < types.size() - 1) std::cout << " | ";
		}
		std::cout << std::endl;
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

	void ArgonLang::RangeExpressionNode::print() const {

	}

	void ArgonLang::RangeExpressionNode::toDot(std::ostream &os, int &nodeId) const {

	}


	void ArgonLang::WhenStatementNode::print() const {

	}

	void ArgonLang::WhenStatementNode::toDot(std::ostream &os, int &nodeId) const {

	}

	void ArgonLang::YieldStatementNode::print() const {

	}

	void ArgonLang::YieldStatementNode::toDot(std::ostream &os, int &nodeId) const {

	}

	void ArgonLang::ParallelExpressionNode::print() const {
	}

	void ArgonLang::ParallelExpressionNode::toDot(std::ostream &os, int &nodeId) const {
		int currentId = nodeId++;
		os << "  node" << currentId << " [label=\"ParallelExpression\"];\n";
		statementNode->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
	}

	void ArgonLang::AwaitExpressionNode::print() const {
		std::cout << "AwaitExpression(";
		statementNode->print();
		std::cout << ")\n";
	}

	void ArgonLang::AwaitExpressionNode::toDot(std::ostream &os, int &nodeId) const {
		int currentId = nodeId++;
		os << "  node" << currentId << " [label=\"AwaitExpression\"];\n";
		statementNode->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
	}

	void ArgonLang::LazyExpressionNode::print() const {

	}

	void ArgonLang::LazyExpressionNode::toDot(std::ostream &os, int &nodeId) const {
		int currentId = nodeId++;
		os << "  node" << currentId << " [label=\"LazyExpression\"];\n";
		statementNode->toDot(os, nodeId);
		os << "  node" << currentId << " -> node" << currentId + 1 << ";\n";
	}

	void ArgonLang::WhileStatementNode::print() const {

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

	void ArgonLang::BreakStatementNode::print() const {

	}

	void ArgonLang::BreakStatementNode::toDot(std::ostream &os, int &nodeId) const {
		int currentId = nodeId++;
		os << "  node" << currentId << " [label=\"BreakStatementNode\"];\n";
	}

	void ArgonLang::ContinueStatementNode::print() const {

	}

	void ArgonLang::ContinueStatementNode::toDot(std::ostream &os, int &nodeId) const {
		int currentId = nodeId++;
		os << "  node" << currentId << " [label=\"ContinueStatementNode\"];\n";
	}

	void ArgonLang::StructExpressionNode::print() const {

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
#endif

ArgonLang::UnionTypeNode::UnionTypeNode(std::vector<std::unique_ptr<TypeNode>> types): types(std::move(types)) {}
ArgonLang::StringLiteralNode::StringLiteralNode(std::string  val) : value(std::move(val)) {}
ArgonLang::IntegralLiteralNode::IntegralLiteralNode(__int128 val, PrimitiveType type) : type(type) {
	switch (type) {
		case PrimitiveType::INT8:   value.i8 = static_cast<int8_t>(val); break;
		case PrimitiveType::INT16:  value.i16 = static_cast<int16_t>(val); break;
		case PrimitiveType::INT32:  value.i32 = static_cast<int32_t>(val); break;
		case PrimitiveType::INT64:  value.i64 = static_cast<int64_t>(val); break;
		case PrimitiveType::INT128: value.i128 = static_cast<__int128>(val); break;
		default: throw std::runtime_error("Invalid type for IntegralLiteralNode");
	}
}

ArgonLang::FloatLiteralNode::FloatLiteralNode(long double val, PrimitiveType type) : type(type) {
	switch (type) {
		case PrimitiveType::FLOAT32:  value.f32 = static_cast<float>(val); break;
		case PrimitiveType::FLOAT64:  value.f64 = static_cast<double>(val); break;
		case PrimitiveType::FLOAT128: value.f128 = static_cast<long double>(val); break;
		default: throw std::runtime_error("Invalid type for FloatLiteralNode");
	}
}
ArgonLang::BooleanLiteralNode::BooleanLiteralNode(bool val) : value(val) {}
ArgonLang::IdentifierNode::IdentifierNode(std::string  val) : identifier(std::move(val)) {}
ArgonLang::BinaryExpressionNode::BinaryExpressionNode(std::unique_ptr<ExpressionNode> lhs, Token operatorSymbol, std::unique_ptr<ExpressionNode> rhs)
        : left(std::move(lhs)), right(std::move(rhs)), op(std::move(operatorSymbol)) {}
ArgonLang::UnaryExpressionNode::UnaryExpressionNode(
		Token operatorSymbol, std::unique_ptr<ExpressionNode> operandNode): op(std::move(operatorSymbol)), operand(std::move(operandNode)) {}

ArgonLang::ProgramNode::ProgramNode(std::vector<std::unique_ptr<ASTNode>> stmts): nodes(std::move(stmts)) {}

ArgonLang::NullExpressionNode::NullExpressionNode() = default;

ArgonLang::FunctionCallExpressionNode::FunctionCallExpressionNode(std::unique_ptr<ExpressionNode> func,
																  std::vector<std::unique_ptr<ExpressionNode>> args): arguments(std::move(args)), function(std::move(func)) {}

ArgonLang::ToExpressionNode::ToExpressionNode(std::unique_ptr<ExpressionNode> lowerBound, std::unique_ptr<ExpressionNode> upperBound, bool isInclusive): lowerBound(std::move(lowerBound)), upperBound(std::move(upperBound)), isInclusive(isInclusive) {}

ArgonLang::LambdaExpressionNode::LambdaExpressionNode(std::unordered_map<std::string, Variable> params,
													  std::unique_ptr<StatementNode> bd): parameters(std::move(params)), body(std::move(bd)) {}

ArgonLang::ComparisonExpressionNode::ComparisonExpressionNode(std::unique_ptr<ExpressionNode> lhs,
															  ArgonLang::Token  operatorSymbol,
															  std::unique_ptr<ExpressionNode> rhs): left(std::move(lhs)),
																									right(std::move(rhs)), op(std::move(operatorSymbol)) {}

ArgonLang::AssignmentExpressionNode::AssignmentExpressionNode(std::unique_ptr<ExpressionNode> lhs,
															  ArgonLang::Token operatorSymbol,
															  std::unique_ptr<ExpressionNode> rhs): left(std::move(lhs)),
																									right(std::move(rhs)), op(std::move(operatorSymbol))  {}

ArgonLang::IndexExpressionNode::IndexExpressionNode(std::unique_ptr<ExpressionNode> arr,
													std::unique_ptr<ExpressionNode> i): array(std::move(arr)), index(std::move(i)) {}

ArgonLang::MatchBranch::MatchBranch(std::unique_ptr<ExpressionNode> pattern, std::unique_ptr<ExpressionNode> condition,
									std::unique_ptr<ExpressionNode> body): pattern(std::move(pattern)), condition(std::move(condition)), body(std::move(body)) {}

ArgonLang::MatchExpressionNode::MatchExpressionNode(std::unique_ptr<ExpressionNode> value,
													std::vector<std::unique_ptr<MatchBranch>> branches): value(std::move(value)), branches(std::move(branches)) {}


ArgonLang::TernaryExpressionNode::TernaryExpressionNode(std::unique_ptr<ExpressionNode> condition,
														std::unique_ptr<ExpressionNode> trueBranch,
														std::unique_ptr<ExpressionNode> falseBranch): condition(std::move(condition)), trueBranch(std::move(trueBranch)), falseBranch(std::move(falseBranch)) {}

ArgonLang::ReturnStatementNode::ReturnStatementNode(std::unique_ptr<ExpressionNode> returnExpression): returnExpression(std::move(returnExpression)) {}

ArgonLang::VariableDeclarationNode::VariableDeclarationNode(bool isConst, std::unique_ptr<TypeNode> type,
															std::unique_ptr<ExpressionNode> value, std::string name): isConst(isConst), type(std::move(type)), value(std::move(value)), name(std::move(name)) {}

ArgonLang::TypeAliasNode::TypeAliasNode(std::string aliasName, std::unique_ptr<TypeNode> targetType): aliasName(std::move(aliasName)), targetType(std::move(targetType)) {}

ArgonLang::IfStatementNode::IfStatementNode(
		std::unique_ptr<ExpressionNode> condition,
		std::unique_ptr<StatementNode> body,
		std::unique_ptr<StatementNode> elseBranch
) : condition(std::move(condition)),
	body(std::move(body)),
	elseBranch(std::move(elseBranch)) {}

ArgonLang::BlockNode::BlockNode(std::vector<std::unique_ptr<ASTNode>> body): body(std::move(body)) {}

ArgonLang::ForStatementNode::ForStatementNode(
		std::string variableName,
		std::unique_ptr<ExpressionNode> iterator,
		std::unique_ptr<StatementNode> body
) : iterator(std::move(iterator)),
	variableName(std::move(variableName)),
	body(std::move(body)) {}

ArgonLang::UnionDeclarationNode::UnionDeclarationNode(
		std::string unionName,
		std::vector<std::unique_ptr<TypeNode>> types
) : unionName(std::move(unionName)), types(std::move(types)) {}

ArgonLang::IdentifierTypeNode::IdentifierTypeNode(std::string typeName)
		: typeName(std::move(typeName)) {}

ArgonLang::RangeExpressionNode::RangeExpressionNode(std::vector<std::unique_ptr<ExpressionNode>> range): range(std::move(range)) {}

ArgonLang::GenericTypeNode::GenericTypeNode(std::string name, std::vector<std::unique_ptr<TypeNode>> params) : name(std::move(name)), params(std::move(params)) {}

ArgonLang::WhenStatementNode::WhenStatementNode(std::string variableName, std::unique_ptr<ExpressionNode> iterator,
												std::unique_ptr<StatementNode> body): variableName(std::move(variableName)), iterator(std::move(iterator)), body(std::move(body)) {}

ArgonLang::YieldStatementNode::YieldStatementNode(std::unique_ptr<ExpressionNode> expressionNode): expressionNode(std::move(expressionNode)) {}

ArgonLang::ParallelExpressionNode::ParallelExpressionNode(std::unique_ptr<ASTNode> statementNode): statementNode(std::move(statementNode)) {}

ArgonLang::AwaitExpressionNode::AwaitExpressionNode(std::unique_ptr<ExpressionNode> statementNode): statementNode(std::move(statementNode)) {}


ArgonLang::LazyExpressionNode::LazyExpressionNode(std::unique_ptr<ExpressionNode> statementNode): statementNode(std::move(statementNode)) {}

ArgonLang::WhileStatementNode::WhileStatementNode(bool isDoWhile, std::unique_ptr<ExpressionNode> condition,
												  std::unique_ptr<StatementNode> body,
												  std::unique_ptr<StatementNode> elseBranch): condition(std::move(condition)), body(std::move(body)), elseBranch(std::move(elseBranch)), isDoWhile(isDoWhile) {}

ArgonLang::StructExpressionNode::StructExpressionNode(
		std::vector<std::tuple<std::string, std::unique_ptr<TypeNode>, std::unique_ptr<ExpressionNode>>> fields): fields(std::move(fields)) {}

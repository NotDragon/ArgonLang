#include "backend/AST.h"

#ifdef DEBUG

void ArgonLang::StringLiteralNode::print() const {
    std::cout << "StringLiteral(" << value << ")";
}

void ArgonLang::IntegralLiteralNode::print() const {
    std::cout << "IntegralLiteral: ";
    switch (type) {
        case PrimitiveType::INT8:   std::cout << value.i8; break;
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
    for (const auto& i : nodes) {
        i->print();
    }
}

void ArgonLang::NullExpressionNode::print() const {
    std::cout << "null";
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

void ArgonLang::FunctionCallExpressionNode::print() const {
    
}

void ArgonLang::LambdaExpressionNode::print() const {
    
}

void ArgonLang::ComparisonExpressionNode::print() const {
    
}

void ArgonLang::AssignmentExpressionNode::print() const {
    
}

void ArgonLang::IndexExpressionNode::print() const {
    
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

void ArgonLang::MatchExpressionNode::print() const {
    std::cout << "Match Expression: \n";
    std::cout << "Value: ";
    if (value) value->print();
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

void ArgonLang::ReturnStatementNode::print() const {

}

void ArgonLang::IntersectionTypeNode::print() const {

}

void ArgonLang::VariableDeclarationNode::print() const {
    
}

void ArgonLang::TypeAliasNode::print() const {
    std::cout << "Type Alias: " << aliasName << " = ";
    if (targetType) targetType->print();
    std::cout << std::endl;
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

void ArgonLang::BlockNode::print() const {
    
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

void ArgonLang::UnionDeclarationNode::print() const {
    std::cout << "Union: " << unionName << " = ";
    for (const auto& type : types) {
        type->print();
        if (&type != &types.back()) std::cout << " | ";
    }
    std::cout << std::endl;
}

void ArgonLang::IdentifierTypeNode::print() const {
    std::cout << "PrimitiveType: " << typeName << std::endl;
}

void ArgonLang::GenericTypeNode::print() const {
    std::cout << "GenericType: " << name << "<";
    for (int i = 0; i < params.size(); ++i) {
        params[i]->print();
        if (i < params.size() - 1) std::cout << ", ";
    }
    std::cout << ">" << std::endl;
}

void ArgonLang::SumTypeNode::print() const {
    std::cout << "SumType: ";
    for (int i = 0; i < types.size(); ++i) {
        types[i]->print();
        if (i < types.size() - 1) std::cout << " | ";
    }
    std::cout << std::endl;
}

void ArgonLang::RangeExpressionNode::print() const {
    
}

void ArgonLang::YieldStatementNode::print() const {
    
}

void ArgonLang::ParallelExpressionNode::print() const {
    
}

void ArgonLang::AwaitExpressionNode::print() const {
    std::cout << "AwaitExpression(";
    statementNode->print();
    std::cout << ")\n";
}

void ArgonLang::LazyExpressionNode::print() const {
    
}

void ArgonLang::WhileStatementNode::print() const {

}

void ArgonLang::BreakStatementNode::print() const {
    
}

void ArgonLang::ContinueStatementNode::print() const {
    
}

void ArgonLang::StructExpressionNode::print() const {
    
}

void ArgonLang::StructField::print() const {

}

void ArgonLang::FunctionArgument::print() const {

}

void ArgonLang::FunctionDeclarationNode::print() const {

}

void ArgonLang::FunctionDefinitionNode::print() const {

}

void ArgonLang::ImplStatementNode::print() const {

}

void ArgonLang::ConstructorStatementNode::ConstructorArgument::print() const {

}

void ArgonLang::ConstructorStatementNode::print() const {

}

void ArgonLang::ClassDeclarationNode::ClassMember::ClassMember::print() const {

}

void ArgonLang::ClassDeclarationNode::print() const {

}

void ArgonLang::MemberAccessExpressionNode::print() const {
	std::cout << "MemberAccessExpressionNode: " << member << std::endl;
}

#endif
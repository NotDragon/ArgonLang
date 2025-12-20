#include <gtest/gtest.h>
#include "backend/AST.h"

using namespace ArgonLang;

// Literal Node Tests
TEST(ASTTests, CreateIntegralLiteralNode) {
	Token::Position pos(1, 1);
	IntegralLiteralNode node(pos, "42", PrimitiveType::INT32);
	EXPECT_EQ(node.type, PrimitiveType::INT32);
	EXPECT_EQ(node.value, "42");
	EXPECT_EQ(node.get_node_type(), ASTNodeType::IntegralLiteral);
}

TEST(ASTTests, CreateFloatLiteralNode) {
	Token::Position pos(1, 1);
	FloatLiteralNode node(pos, "3.14", PrimitiveType::FLOAT32);
	EXPECT_EQ(node.type, PrimitiveType::FLOAT32);
	EXPECT_EQ(node.value, "3.14");
	EXPECT_EQ(node.get_node_type(), ASTNodeType::FloatLiteral);
}

TEST(ASTTests, CreateStringLiteralNode) {
	Token::Position pos(1, 1);
	StringLiteralNode node(pos, "hello");
	EXPECT_EQ(node.value, "hello");
	EXPECT_EQ(node.get_node_type(), ASTNodeType::StringLiteral);
}

TEST(ASTTests, CreateBooleanLiteralNode) {
	Token::Position pos(1, 1);
	BooleanLiteralNode node(pos, true);
	EXPECT_EQ(node.value, true);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::BooleanLiteral);
}

// Expression Node Tests
TEST(ASTTests, CreateBinaryExpressionNode) {
	Token::Position pos(1, 1);
	auto left = std::make_unique<IntegralLiteralNode>(pos, "42", PrimitiveType::INT32);
	auto right = std::make_unique<IntegralLiteralNode>(pos, "24", PrimitiveType::INT32);
	Token op(Token::Plus, "+", 1, 1);

	BinaryExpressionNode node(pos, std::move(left), op, std::move(right));
	EXPECT_EQ(node.op.type, Token::Plus);
	EXPECT_NE(node.left, nullptr);
	EXPECT_NE(node.right, nullptr);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::BinaryExpression);
}

TEST(ASTTests, CreateUnaryExpressionNode) {
	Token::Position pos(1, 1);
	auto operand = std::make_unique<IntegralLiteralNode>(pos, "42", PrimitiveType::INT32);
	Token op(Token::Minus, "-", 1, 1);

	UnaryExpressionNode node(pos, op, std::move(operand));
	EXPECT_EQ(node.op.type, Token::Minus);
	EXPECT_NE(node.operand, nullptr);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::UnaryExpression);
}

TEST(ASTTests, CreateIdentifierNode) {
	Token::Position pos(1, 1);
	IdentifierNode node(pos, "myVariable");
	EXPECT_EQ(node.identifier, "myVariable");
	EXPECT_EQ(node.get_node_type(), ASTNodeType::Identifier);
}

TEST(ASTTests, CreateFunctionCallNode) {
	Token::Position pos(1, 1);
	auto callee = std::make_unique<IdentifierNode>(pos, "add");
	std::vector<std::unique_ptr<ExpressionNode>> args;
	args.push_back(std::make_unique<IntegralLiteralNode>(pos, "5", PrimitiveType::INT32));
	args.push_back(std::make_unique<IntegralLiteralNode>(pos, "3", PrimitiveType::INT32));

	FunctionCallExpressionNode node(pos, std::move(callee), std::move(args));
	EXPECT_EQ(node.arguments.size(), 2);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::FunctionCallExpression);
}

// Type Node Tests
TEST(ASTTests, CreateIdentifierTypeNode) {
	Token::Position pos(1, 1);
	IdentifierTypeNode node(pos, "i32");
	EXPECT_EQ(node.typeName, "i32");
	EXPECT_EQ(node.get_node_type(), ASTNodeType::IdentifierType);
}

TEST(ASTTests, CreateGenericTypeNode) {
	Token::Position pos(1, 1);
	auto baseType = std::make_unique<IdentifierTypeNode>(pos, "vec");
	std::vector<std::unique_ptr<TypeNode>> typeArgs;
	typeArgs.push_back(std::make_unique<IdentifierTypeNode>(pos, "i32"));

	GenericTypeNode node(pos, std::move(baseType), std::move(typeArgs));
	EXPECT_EQ(node.params.size(), 1);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::GenericType);
}

TEST(ASTTests, CreateFunctionTypeNode) {
	Token::Position pos(1, 1);
	std::vector<std::unique_ptr<TypeNode>> paramTypes;
	paramTypes.push_back(std::make_unique<IdentifierTypeNode>(pos, "i32"));
	paramTypes.push_back(std::make_unique<IdentifierTypeNode>(pos, "i32"));
	auto returnType = std::make_unique<IdentifierTypeNode>(pos, "i32");

	FunctionTypeNode node(pos, std::move(paramTypes), std::move(returnType));
	EXPECT_EQ(node.parameterTypes.size(), 2);
	EXPECT_NE(node.returnType, nullptr);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::FunctionType);
}

// Statement Node Tests
TEST(ASTTests, CreateVariableDeclarationNode) {
	Token::Position pos(1, 1);
	auto type = std::make_unique<IdentifierTypeNode>(pos, "i32");
	auto initializer = std::make_unique<IntegralLiteralNode>(pos, "42", PrimitiveType::INT32);

	VariableDeclarationNode node(pos, false, std::move(type), std::move(initializer), "x");
	EXPECT_EQ(node.name, "x");
	EXPECT_NE(node.type, nullptr);
	EXPECT_NE(node.value, nullptr);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::VariableDeclaration);
}

TEST(ASTTests, CreateReturnStatementNode) {
	Token::Position pos(1, 1);
	auto value = std::make_unique<IntegralLiteralNode>(pos, "42", PrimitiveType::INT32);

	ReturnStatementNode node(pos, std::move(value), false);
	EXPECT_NE(node.returnExpression, nullptr);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::ReturnStatement);
}

TEST(ASTTests, CreateBlockNode) {
	Token::Position pos(1, 1);
	std::vector<std::unique_ptr<ASTNode>> statements;
	statements.push_back(std::make_unique<IntegralLiteralNode>(pos, "42", PrimitiveType::INT32));
	statements.push_back(std::make_unique<IntegralLiteralNode>(pos, "24", PrimitiveType::INT32));

	BlockNode node(pos, std::move(statements));
	EXPECT_EQ(node.body.size(), 2);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::Block);
}

// Complex Node Tests
TEST(ASTTests, CreateFunctionDeclarationNode) {
	Token::Position pos(1, 1);
	std::vector<std::unique_ptr<FunctionArgument>> args;
	auto returnType = std::make_unique<IdentifierTypeNode>(pos, "i32");
	auto body = std::make_unique<BlockNode>(pos, std::vector<std::unique_ptr<ASTNode>>());
	auto name = std::make_unique<IdentifierNode>(pos, "testFunc");

	FunctionDeclarationNode node(pos, std::move(returnType), std::move(args), std::move(body), std::move(name));
	EXPECT_EQ(node.args.size(), 0);
	EXPECT_NE(node.returnType, nullptr);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::FunctionDeclaration);
}

TEST(ASTTests, CreateClassDeclarationNode) {
	Token::Position pos(1, 1);
	std::vector<ClassDeclarationNode::ClassMember> members;

	ClassDeclarationNode node(pos, "TestClass", std::move(members));
	EXPECT_EQ(node.className, "TestClass");
	EXPECT_EQ(node.body.size(), 0);
	EXPECT_EQ(node.get_node_type(), ASTNodeType::ClassDeclaration);
}

// AST Node Type String Tests
TEST(ASTTests, ASTNodeTypeToString) {
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::IntegralLiteral), "IntegralLiteral");
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::FloatLiteral), "FloatLiteral");
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::StringLiteral), "StringLiteral");
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::BooleanLiteral), "BooleanLiteral");
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::Identifier), "Identifier");
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::BinaryExpression), "BinaryExpression");
	EXPECT_EQ(ast_node_type_to_string(ASTNodeType::UnaryExpression), "UnaryExpression");
}

// Primitive Type Tests
TEST(ASTTests, PrimitiveTypeToString) {
	EXPECT_EQ(primitive_type_to_string(PrimitiveType::INT32), "i32");
	EXPECT_EQ(primitive_type_to_string(PrimitiveType::INT64), "i64");
	EXPECT_EQ(primitive_type_to_string(PrimitiveType::FLOAT32), "f32");
	EXPECT_EQ(primitive_type_to_string(PrimitiveType::FLOAT64), "f64");
}

// Edge Cases and Null Tests
TEST(ASTTests, CreateNodeWithEmptyStrings) {
	Token::Position pos(1, 1);
	IdentifierNode node(pos, "");
	EXPECT_EQ(node.identifier, "");
}

TEST(ASTTests, CreateNodeWithLargeNumbers) {
	Token::Position pos(1, 1);
	IntegralLiteralNode node(pos, "2147483647", PrimitiveType::INT32);
	EXPECT_EQ(node.value, "2147483647");
}

TEST(ASTTests, CreateComplexNestedExpression) {
	Token::Position pos(1, 1);
	
	// Create: (5 + 3) * 2
	auto left = std::make_unique<IntegralLiteralNode>(pos, "5", PrimitiveType::INT32);
	auto right = std::make_unique<IntegralLiteralNode>(pos, "3", PrimitiveType::INT32);
	Token plusOp(Token::Plus, "+", 1, 1);
	auto addExpr = std::make_unique<BinaryExpressionNode>(pos, std::move(left), plusOp, std::move(right));
	
	auto two = std::make_unique<IntegralLiteralNode>(pos, "2", PrimitiveType::INT32);
	Token multOp(Token::Multiply, "*", 1, 1);
	BinaryExpressionNode mulExpr(pos, std::move(addExpr), multOp, std::move(two));
	
	EXPECT_NE(mulExpr.left, nullptr);
	EXPECT_NE(mulExpr.right, nullptr);
	EXPECT_EQ(mulExpr.op.type, Token::Multiply);
}

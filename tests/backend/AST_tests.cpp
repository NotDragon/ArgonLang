#include <gtest/gtest.h>
#include "backend/AST.h"

TEST(ASTTests, CreateIntegralLiteralNode) {
	ArgonLang::IntegralLiteralNode node(42, ArgonLang::PrimitiveType::INT32);
	EXPECT_EQ(node.type, ArgonLang::PrimitiveType::INT32);
	EXPECT_EQ(node.value.i32, 42);
}

TEST(ASTTests, CreateBinaryExpressionNode) {
	using namespace ArgonLang;

	auto left = std::make_unique<IntegralLiteralNode>(42, PrimitiveType::INT32);
	auto right = std::make_unique<IntegralLiteralNode>(24, PrimitiveType::INT32);
	Token op(Token::Plus, "+", 1, 1);

	BinaryExpressionNode node(std::move(left), op, std::move(right));
	EXPECT_EQ(node.op.type, Token::Plus);
	EXPECT_NE(node.left, nullptr);
	EXPECT_NE(node.right, nullptr);
}

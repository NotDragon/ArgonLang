#include <gtest/gtest.h>
#include "backend/AST.h"

TEST(ASTTests, CreateIntegralLiteralNode) {
	ArgonLang::Token::Position pos(1, 1);
	ArgonLang::IntegralLiteralNode node(pos, 42, ArgonLang::PrimitiveType::INT32);
	EXPECT_EQ(node.type, ArgonLang::PrimitiveType::INT32);
	EXPECT_EQ(node.value.i32, 42);
}

TEST(ASTTests, CreateBinaryExpressionNode) {
	using namespace ArgonLang;

	Token::Position pos(1, 1);
	auto left = std::make_unique<IntegralLiteralNode>(pos, 42, PrimitiveType::INT32);
	auto right = std::make_unique<IntegralLiteralNode>(pos, 24, PrimitiveType::INT32);
	Token op(Token::Plus, "+", 1, 1);

	BinaryExpressionNode node(pos, std::move(left), op, std::move(right));
	EXPECT_EQ(node.op.type, Token::Plus);
	EXPECT_NE(node.left, nullptr);
	EXPECT_NE(node.right, nullptr);
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"

TEST(ParserTests, ParseVariableDeclaration) {
	std::string input = "def x: i32 = 42;";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error()) << "Tokenization failed: " << tokenizeResult.error_msg;
	
	ArgonLang::Parser parser(tokenizeResult.tokens);

	auto result = parser.parse();
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());

	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::VariableDeclarationNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->name, "x");
}

TEST(ParserTests, ParseFunctionDeclaration) {
	std::string input = "func add(a: i32, b: i32) i32 -> a + b;";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error()) << "Tokenization failed: " << tokenizeResult.error_msg;
	
	ArgonLang::Parser parser(tokenizeResult.tokens);

	auto result = parser.parse();
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());

	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->args.size(), 2);
	EXPECT_EQ(node->returnType->get_node_type(), ArgonLang::ASTNodeType::IdentifierType);
}
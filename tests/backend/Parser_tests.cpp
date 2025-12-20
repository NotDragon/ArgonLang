#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"

// Variable Declaration Tests
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

TEST(ParserTests, ParseVariableWithoutInitializer) {
	std::string input = "def y: f64;";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

TEST(ParserTests, ParseMultipleVariables) {
	std::string input = "def x: i32 = 10; def y: f64 = 3.14;";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
    auto program = std::move(result.value());
	EXPECT_EQ(program->nodes.size(), 2);
}

// Function Declaration Tests
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

TEST(ParserTests, ParseFunctionWithBody) {
	std::string input = "func square(x: i32) i32 { return x * x; }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

TEST(ParserTests, ParseFunctionNoParameters) {
	std::string input = "func getZero() i32 { return 0; }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
    auto program = std::move(result.value());
	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->args.size(), 0);
}

// Class Declaration Tests
TEST(ParserTests, ParseBasicClass) {
	std::string input = "class Point { pub x: i32; pub y: i32; }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
    auto program = std::move(result.value());
	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::ClassDeclarationNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
	EXPECT_EQ(node->className, "Point");
}

TEST(ParserTests, ParseClassWithMethod) {
	std::string input = "class Counter { pub value: i32; pub func inc() void { value = value + 1; } }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

// Enum Declaration Tests
TEST(ParserTests, ParseBasicEnum) {
	std::string input = "enum Color { Red, Green, Blue }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
    auto program = std::move(result.value());
	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::EnumDeclarationNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
}

// Union Declaration Tests
TEST(ParserTests, ParseBasicUnion) {
	std::string input = "union Number { int_val: i32, float_val: f64 }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
    auto program = std::move(result.value());
	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::UnionDeclarationNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
}

// Type Alias Tests
TEST(ParserTests, ParseTypeAlias) {
	std::string input = "using Integer = i32;";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
    auto program = std::move(result.value());
	ASSERT_EQ(program->nodes.size(), 1);
	auto node = dynamic_cast<ArgonLang::TypeAliasNode*>(program->nodes[0].get());
	ASSERT_NE(node, nullptr);
}

// Control Flow Tests
TEST(ParserTests, ParseIfStatement) {
	std::string input = "func test() i32 { if (true) { return 1; } return 0; }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

TEST(ParserTests, ParseWhileLoop) {
	std::string input = "func test() void { while (true) { } }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

TEST(ParserTests, ParseForLoop) {
	std::string input = "func test() void { for (i: i32 -> 0 to 10) { } }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

// Expression Tests
TEST(ParserTests, ParseBinaryExpression) {
	std::string input = "func test() i32 { return 5 + 3 * 2; }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

TEST(ParserTests, ParseFunctionCall) {
	std::string input = "func test() i32 { return add(5, 3); }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

TEST(ParserTests, ParseMemberAccess) {
	std::string input = "func test(p: Point) i32 { return p.x; }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	ASSERT_FALSE(tokenizeResult.has_error());
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    ASSERT_TRUE(result.has_value());
}

// Error Cases
TEST(ParserTests, ParseErrorMissingSemicolon) {
	std::string input = "def x: i32 = 42";
	auto tokenizeResult = ArgonLang::tokenize(input);
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    EXPECT_FALSE(result.has_value());
}

TEST(ParserTests, ParseErrorInvalidSyntax) {
	std::string input = "func (a: i32) i32 { }";
	auto tokenizeResult = ArgonLang::tokenize(input);
	
	ArgonLang::Parser parser(tokenizeResult.tokens);
	auto result = parser.parse();
    EXPECT_FALSE(result.has_value());
}
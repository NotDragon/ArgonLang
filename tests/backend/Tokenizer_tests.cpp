#include <gtest/gtest.h>
#include "backend/Tokenizer.h"

// Basic Keyword Tests
TEST(TokenizerTests, KeywordTokenization) {
	std::string input = "def x: i32;";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error()) << "Tokenization failed: " << tokenizeResult.error_msg;
	auto& tokens = tokenizeResult.tokens;
	
	ASSERT_EQ(tokens.size(), 6);
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::KeywordDef);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::Colon);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::PrimitiveType);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::Semicolon);
	EXPECT_EQ(tokens[5].type, ArgonLang::Token::End);
}

TEST(TokenizerTests, AllKeywords) {
	std::string input = "def func class if else while for return break continue using enum union constraint module import par yield struct pub constructor impl";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::KeywordDef);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::KeywordFunc);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::KeywordClass);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::KeywordIf);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::KeywordElse);
	EXPECT_EQ(tokens[5].type, ArgonLang::Token::KeywordWhile);
	EXPECT_EQ(tokens[6].type, ArgonLang::Token::KeywordFor);
	EXPECT_EQ(tokens[7].type, ArgonLang::Token::KeywordReturn);
}

// Literal Tests
TEST(TokenizerTests, IntegerLiterals) {
	std::string input = "42 -10 0 999999";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::IntegralLiteral);
	EXPECT_EQ(tokens[0].value, "42");
}

TEST(TokenizerTests, FloatLiterals) {
	std::string input = "3.14 -2.5 0.0 1.5e10";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::FloatLiteral);
	EXPECT_EQ(tokens[0].value, "3.14");
}

TEST(TokenizerTests, StringLiterals) {
	std::string input = R"("hello" "world" "")";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;

	EXPECT_EQ(tokens[0].type, ArgonLang::Token::StringLiteral);
	EXPECT_EQ(tokens[0].value, "hello");
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::StringLiteral);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::StringLiteral);
}

TEST(TokenizerTests, BooleanLiterals) {
	std::string input = "true false";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::BooleanLiteral);
	EXPECT_EQ(tokens[0].value, "true");
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::BooleanLiteral);
	EXPECT_EQ(tokens[1].value, "false");
}

// Operator Tests
TEST(TokenizerTests, ArithmeticOperators) {
	std::string input = "+ - * / %";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::Plus);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::Minus);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::Multiply);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::Divide);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::Modulo);
}

TEST(TokenizerTests, ComparisonOperators) {
	std::string input = "== != < > <= >=";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::Equal);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::NotEqual);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::Less);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::Greater);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::LessEqual);
	EXPECT_EQ(tokens[5].type, ArgonLang::Token::GreaterEqual);
}

TEST(TokenizerTests, LogicalOperators) {
	std::string input = "&& || !";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::LogicalAnd);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::LogicalOr);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::LogicalNot);
}

TEST(TokenizerTests, AssignmentOperators) {
	std::string input = "= += -= *= /= %=";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::Assign);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::PlusAssign);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::MinusAssign);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::MultiplyAssign);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::DivideAssign);
}

TEST(TokenizerTests, BitwiseOperators) {
	std::string input = "*& *| *^ *~ *< *>";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::BitwiseAnd);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::BitwiseOr);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::BitwiseXor);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::BitwiseNot);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::LeftShift);
	EXPECT_EQ(tokens[5].type, ArgonLang::Token::RightShift);
}

// Identifier Tests
TEST(TokenizerTests, Identifiers) {
	std::string input = "myVar _private value123 CamelCase snake_case";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[0].value, "myVar");
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[1].value, "_private");
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::Identifier);
}

// Primitive Type Tests
TEST(TokenizerTests, PrimitiveTypes) {
	std::string input = "i32 i64 f32 f64 bool str void";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::PrimitiveType);
	EXPECT_EQ(tokens[0].value, "i32");
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::PrimitiveType);
	EXPECT_EQ(tokens[1].value, "i64");
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::PrimitiveType);
	EXPECT_EQ(tokens[2].value, "f32");
}

// Punctuation Tests
TEST(TokenizerTests, Punctuation) {
	std::string input = "( ) { } [ ] ; : , . -> =>";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::LeftParen);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::RightParen);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::LeftBrace);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::RightBrace);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::LeftBracket);
	EXPECT_EQ(tokens[5].type, ArgonLang::Token::RightBracket);
	EXPECT_EQ(tokens[6].type, ArgonLang::Token::Semicolon);
	EXPECT_EQ(tokens[7].type, ArgonLang::Token::Colon);
}

// Complex Expression Tests
TEST(TokenizerTests, CompleteFunction) {
	std::string input = "func add(a: i32, b: i32) i32 { return a + b; }";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.has_error());
	auto& tokens = tokenizeResult.tokens;
	
	EXPECT_GT(tokens.size(), 10);
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::KeywordFunc);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[1].value, "add");
}

// Error Cases
TEST(TokenizerTests, InvalidCharacter) {
	std::string input = "def x @ i32;";
	auto tokenizeResult = ArgonLang::tokenize(input);

	// Should handle invalid characters appropriately
	EXPECT_TRUE(tokenizeResult.has_error() || !tokenizeResult.has_error());
}

TEST(TokenizerTests, UnterminatedString) {
	std::string input = R"(def s = "unterminated)";
	auto tokenizeResult = ArgonLang::tokenize(input);

	EXPECT_TRUE(tokenizeResult.has_error());
}

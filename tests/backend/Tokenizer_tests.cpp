#include <gtest/gtest.h>
#include "backend/Tokenizer.h"

TEST(TokenizerTests, KeywordTokenization) {
	std::string input = "def x: i32;";
	auto tokenizeResult = ArgonLang::tokenize(input);

	ASSERT_FALSE(tokenizeResult.hasError()) << "Tokenization failed: " << tokenizeResult.errorMsg;
	auto& tokens = tokenizeResult.tokens;
	
	ASSERT_EQ(tokens.size(), 6); // Number of tokens
	EXPECT_EQ(tokens[0].type, ArgonLang::Token::KeywordDef);
	EXPECT_EQ(tokens[1].type, ArgonLang::Token::Identifier);
	EXPECT_EQ(tokens[2].type, ArgonLang::Token::Colon);
	EXPECT_EQ(tokens[3].type, ArgonLang::Token::PrimitiveType);
	EXPECT_EQ(tokens[4].type, ArgonLang::Token::Semicolon);
	EXPECT_EQ(tokens[5].type, ArgonLang::Token::End);
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class ConstraintsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    
    ArgonLang::Result<std::unique_ptr<ArgonLang::ProgramNode>> parseCode(const std::string& input) {
        auto tokenizeResult = ArgonLang::tokenize(input);
        if (tokenizeResult.hasError()) {
            return ArgonLang::Err<std::unique_ptr<ArgonLang::ProgramNode>>(ArgonLang::create_parse_error(ArgonLang::ErrorType::InvalidExpression, tokenizeResult.errorMsg, ArgonLang::Token::Position()));
        }
        
        ArgonLang::Parser parser(tokenizeResult.tokens);
        return parser.parse();
    }
    
    std::string generateCode(const std::string& input) {
        auto parseResult = parseCode(input);
        if (!parseResult.has_value()) {
            return "PARSE_ERROR: " + parseResult.error().message;
        }
        
        ArgonLang::CodeGenerationVisitor visitor;
        auto codeResult = visitor.visit(*parseResult.value());
        if (!codeResult.has_value()) {
            return "CODEGEN_ERROR: " + codeResult.error().message;
        }
        
        return codeResult.value();
    }
};

// Basic Constraint Declaration Tests
TEST_F(ConstraintsTest, ParseSimpleConstraint) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    
    EXPECT_EQ(constraintNode->constraintName, "Positive");
    EXPECT_EQ(constraintNode->genericParams.size(), 1);
    EXPECT_EQ(constraintNode->genericParams[0]->name, "T");
}

TEST_F(ConstraintsTest, ParseConstraintWithMultipleParameters) {
    std::string input = "constraint InRange<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    
    EXPECT_EQ(constraintNode->constraintName, "InRange");
    EXPECT_EQ(constraintNode->genericParams.size(), 3);
    EXPECT_EQ(constraintNode->genericParams[0]->name, "T");
    EXPECT_EQ(constraintNode->genericParams[1]->name, "Min");
    EXPECT_EQ(constraintNode->genericParams[2]->name, "Max");
}

TEST_F(ConstraintsTest, ParseConstraintWithStringType) {
    std::string input = "constraint NonEmptyString<T: str> = T.length > 0;";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    
    EXPECT_EQ(constraintNode->constraintName, "NonEmptyString");
    EXPECT_EQ(constraintNode->genericParams.size(), 1);
    EXPECT_EQ(constraintNode->genericParams[0]->name, "T");
}

// Constraint Code Generation Tests
TEST_F(ConstraintsTest, GenerateSimpleConstraint) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("// Constraint Positive") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
}

TEST_F(ConstraintsTest, GenerateConstraintWithConcreteTypes) {
    std::string input = "constraint ValidRange<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("// Constraint ValidRange") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T, typename Min, typename Max>") != std::string::npos);
    EXPECT_TRUE(code.find("requires Number<T> && std::same_as<Min, int32_t> && std::same_as<Max, int32_t>") != std::string::npos);
    EXPECT_TRUE(code.find("concept ValidRange = T >= Min && T <= Max;") != std::string::npos);
}

TEST_F(ConstraintsTest, GenerateConstraintWithStringType) {
    std::string input = "constraint NonEmptyString<T: str> = T.length > 0;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("// Constraint NonEmptyString") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires std::same_as<T, str>") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonEmptyString = T.length > 0;") != std::string::npos);
}

// Constraint Usage in Function Signatures Tests
TEST_F(ConstraintsTest, ParseFunctionWithConstrainedParameter) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        func<T: Number> processPositive(value: T & Positive) T {
            return value * 2;
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 2);
    
    // Check constraint declaration
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    EXPECT_EQ(constraintNode->constraintName, "Positive");
    
    // Check function declaration
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[1].get());
    ASSERT_NE(funcNode, nullptr);
    EXPECT_EQ(funcNode->genericParams.size(), 1);
}

TEST_F(ConstraintsTest, GenerateFunctionWithConstrainedParameter) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        func<T: Number> processPositive(value: T & Positive) T {
            return value * 2;
        }
    )";
    std::string code = generateCode(input);
    
    // Check constraint generation
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    
    // Check function generation - intersection type should be erased to base type
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T processPositive(T value)") != std::string::npos);
}

// Complex Constraint Expressions Tests
TEST_F(ConstraintsTest, ParseComplexConstraintExpression) {
    std::string input = "constraint EvenPositive<T: Number> = T > 0 && T % 2 == 0;";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    
    EXPECT_EQ(constraintNode->constraintName, "EvenPositive");
}

TEST_F(ConstraintsTest, GenerateComplexConstraintExpression) {
    std::string input = "constraint EvenPositive<T: Number> = T > 0 && T % 2 == 0;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("concept EvenPositive = T > 0 && T % 2 == 0;") != std::string::npos);
}

// Multiple Constraints Tests
TEST_F(ConstraintsTest, ParseMultipleConstraints) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        constraint Even<T: Number> = T % 2 == 0;
    )";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 3);
    
    auto constraint1 = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraint1, nullptr);
    EXPECT_EQ(constraint1->constraintName, "Positive");
    
    auto constraint2 = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[1].get());
    ASSERT_NE(constraint2, nullptr);
    EXPECT_EQ(constraint2->constraintName, "NonZero");
    
    auto constraint3 = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[2].get());
    ASSERT_NE(constraint3, nullptr);
    EXPECT_EQ(constraint3->constraintName, "Even");
}

TEST_F(ConstraintsTest, GenerateMultipleConstraints) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        constraint Even<T: Number> = T % 2 == 0;
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Even = T % 2 == 0;") != std::string::npos);
}

// Built-in Concepts Tests
TEST_F(ConstraintsTest, GenerateBuiltInConcepts) {
    std::string input = "func<T: Number> test(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    // Check that built-in concepts are generated
    EXPECT_TRUE(code.find("template<typename T>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
}

// Error Handling Tests
TEST_F(ConstraintsTest, ParseErrorMissingConstraintName) {
    std::string input = "constraint <T: Number> = T > 0;";
    auto result = parseCode(input);
    
    EXPECT_TRUE(!result.has_value());
}

TEST_F(ConstraintsTest, ParseErrorMissingConstraintExpression) {
    std::string input = "constraint Positive<T: Number> =;";
    auto result = parseCode(input);
    
    EXPECT_TRUE(!result.has_value());
}

TEST_F(ConstraintsTest, ParseErrorMissingSemicolon) {
    std::string input = "constraint Positive<T: Number> = T > 0";
    auto result = parseCode(input);
    
    EXPECT_TRUE(!result.has_value());
}

// Intersection Types with Constraints Tests
TEST_F(ConstraintsTest, ParseIntersectionTypeWithMultipleConstraints) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint Even<T: Number> = T % 2 == 0;
        func process(value: i32 & Positive & Even) i32 {
            return value * 2;
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_FALSE(!result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 3);
}

TEST_F(ConstraintsTest, GenerateIntersectionTypeWithMultipleConstraints) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint Even<T: Number> = T % 2 == 0;
        func process(value: i32 & Positive & Even) i32 {
            return value * 2;
        }
    )";
    std::string code = generateCode(input);
    
    // Intersection types should be erased to base type
    EXPECT_TRUE(code.find("int32_t process(int32_t value)") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Even = T % 2 == 0;") != std::string::npos);
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class GenericsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    
    ArgonLang::Result<std::unique_ptr<ArgonLang::ProgramNode>> parseCode(const std::string& input) {
        auto tokenizeResult = ArgonLang::tokenize(input);
        if (tokenizeResult.has_error()) {
            return ArgonLang::Err<std::unique_ptr<ArgonLang::ProgramNode>>(ArgonLang::create_parse_error(ArgonLang::ErrorType::InvalidExpression, tokenizeResult.error_msg, ArgonLang::Token::Position()));
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

// Generic Function Declaration Tests
TEST_F(GenericsTest, ParseGenericFunctionSingleParameter) {
    std::string input = "func<T: Type> identity(x: T) T { return x; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    EXPECT_EQ(funcNode->genericParams.size(), 1);
    EXPECT_EQ(funcNode->genericParams[0]->name, "T");
    ASSERT_NE(funcNode->genericParams[0]->constraint, nullptr);
}

TEST_F(GenericsTest, ParseGenericFunctionMultipleParameters) {
    std::string input = "func<T: Number, U: Type> combine(a: T, b: U) T { return a; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    EXPECT_EQ(funcNode->genericParams.size(), 2);
    EXPECT_EQ(funcNode->genericParams[0]->name, "T");
    EXPECT_EQ(funcNode->genericParams[1]->name, "U");
}

TEST_F(GenericsTest, ParseGenericFunctionConcreteConstraint) {
    std::string input = "func<T: i32> processInt(x: T) T { return x; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    EXPECT_EQ(funcNode->genericParams.size(), 1);
    EXPECT_EQ(funcNode->genericParams[0]->name, "T");
}

// Generic Function Code Generation Tests
TEST_F(GenericsTest, GenerateGenericFunctionSingleConstraint) {
    std::string input = "func<T: Number> square(x: T) T { return x * x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T square(T x)") != std::string::npos);
}

TEST_F(GenericsTest, GenerateGenericFunctionMultipleConstraints) {
    std::string input = "func<T: Number, U: Type> process(a: T, b: U) T { return a; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Type<U>") != std::string::npos);
    EXPECT_TRUE(code.find("T process(T a,U b)") != std::string::npos);
}

TEST_F(GenericsTest, GenerateGenericFunctionConcreteConstraint) {
    std::string input = "func<T: i32> processInt(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T> requires std::same_as<T, int32_t>") != std::string::npos);
    EXPECT_TRUE(code.find("T processInt(T x)") != std::string::npos);
}

// Generic Class Declaration Tests
TEST_F(GenericsTest, ParseGenericClassSingleParameter) {
    std::string input = "class Vec<T: Type> { pub data: T; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto classNode = dynamic_cast<ArgonLang::ClassDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(classNode, nullptr);
    
    EXPECT_EQ(classNode->className, "Vec");
    EXPECT_EQ(classNode->genericParams.size(), 1);
    EXPECT_EQ(classNode->genericParams[0]->name, "T");
}

TEST_F(GenericsTest, ParseGenericClassMultipleParameters) {
    std::string input = "class Map<K: Type, V: Type> { pub key: K; pub value: V; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto classNode = dynamic_cast<ArgonLang::ClassDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(classNode, nullptr);
    
    EXPECT_EQ(classNode->className, "Map");
    EXPECT_EQ(classNode->genericParams.size(), 2);
    EXPECT_EQ(classNode->genericParams[0]->name, "K");
    EXPECT_EQ(classNode->genericParams[1]->name, "V");
}

// Generic Class Code Generation Tests
TEST_F(GenericsTest, GenerateGenericClassSingleConstraint) {
    std::string input = "class Container<T: Type> { pub value: T; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
    EXPECT_TRUE(code.find("class Container{") != std::string::npos);
    EXPECT_TRUE(code.find("public:T value;") != std::string::npos);
}

TEST_F(GenericsTest, GenerateGenericClassMultipleConstraints) {
    std::string input = "class Pair<T: Number, U: Type> { pub first: T; pub second: U; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Type<U>") != std::string::npos);
    EXPECT_TRUE(code.find("class Pair{") != std::string::npos);
    EXPECT_TRUE(code.find("public:T first;") != std::string::npos);
    EXPECT_TRUE(code.find("public:U second;") != std::string::npos);
}

// Generic Constraint Declaration Tests
TEST_F(GenericsTest, ParseGenericConstraintSingleParameter) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    
    EXPECT_EQ(constraintNode->constraintName, "Positive");
    EXPECT_EQ(constraintNode->genericParams.size(), 1);
    EXPECT_EQ(constraintNode->genericParams[0]->name, "T");
}

TEST_F(GenericsTest, ParseGenericConstraintMultipleParameters) {
    std::string input = "constraint Range<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    
    EXPECT_EQ(constraintNode->constraintName, "Range");
    EXPECT_EQ(constraintNode->genericParams.size(), 3);
    EXPECT_EQ(constraintNode->genericParams[0]->name, "T");
    EXPECT_EQ(constraintNode->genericParams[1]->name, "Min");
    EXPECT_EQ(constraintNode->genericParams[2]->name, "Max");
}

// Generic Constraint Code Generation Tests
TEST_F(GenericsTest, GenerateGenericConstraintSingleParameter) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("// Constraint Positive") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
}

TEST_F(GenericsTest, GenerateGenericConstraintMultipleParameters) {
    std::string input = "constraint Range<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("// Constraint Range") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T, typename Min, typename Max> requires Number<T> && std::same_as<Min, int32_t> && std::same_as<Max, int32_t>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Range = T >= Min && T <= Max;") != std::string::npos);
}

// Error Handling Tests
TEST_F(GenericsTest, ParseErrorMissingGenericParameterName) {
    std::string input = "func<: Number> invalid() -> void {}";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(GenericsTest, ParseErrorMissingClosingAngleBracket) {
    std::string input = "func<T: Number invalid() -> void {}";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(GenericsTest, ParseErrorInvalidConstraintSyntax) {
    std::string input = "func<T:> invalid() -> void {}";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

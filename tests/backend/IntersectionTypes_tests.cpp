#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class IntersectionTypesTest : public ::testing::Test {
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

// Basic Intersection Type Tests
TEST_F(IntersectionTypesTest, ParseSimpleIntersectionType) {
    std::string input = "func process(value: i32 & Positive) i32 { return value; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    ASSERT_EQ(funcNode->args.size(), 1);
    auto argType = funcNode->args[0]->type.get();
    auto intersectionType = dynamic_cast<ArgonLang::IntersectionTypeNode*>(argType);
    ASSERT_NE(intersectionType, nullptr);
    
    EXPECT_EQ(intersectionType->types.size(), 2);
}

TEST_F(IntersectionTypesTest, ParseMultipleIntersectionTypes) {
    std::string input = "func process(value: i32 & Positive & NonZero & Even) i32 { return value; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    ASSERT_EQ(funcNode->args.size(), 1);
    auto argType = funcNode->args[0]->type.get();
    auto intersectionType = dynamic_cast<ArgonLang::IntersectionTypeNode*>(argType);
    ASSERT_NE(intersectionType, nullptr);
    
    EXPECT_EQ(intersectionType->types.size(), 4);
}

TEST_F(IntersectionTypesTest, ParseIntersectionTypeInReturnType) {
    std::string input = "func create() i32 & Positive { return 42; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    auto returnType = funcNode->returnType.get();
    auto intersectionType = dynamic_cast<ArgonLang::IntersectionTypeNode*>(returnType);
    ASSERT_NE(intersectionType, nullptr);
    
    EXPECT_EQ(intersectionType->types.size(), 2);
}

// Type Erasure Code Generation Tests
TEST_F(IntersectionTypesTest, GenerateSimpleIntersectionTypeErasure) {
    std::string input = "func process(value: i32 & Positive) i32 { return value; }";
    std::string code = generateCode(input);
    
    // Intersection type should be erased to base type (i32 int32_t)
    EXPECT_TRUE(code.find("int32_t process(int32_t value)") != std::string::npos);
    // Should not contain tuple or std::tuple
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
    EXPECT_TRUE(code.find("std::tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateMultipleIntersectionTypeErasure) {
    std::string input = "func process(value: i32 & Positive & NonZero & Even) i32 { return value; }";
    std::string code = generateCode(input);
    
    // Should erase to base type
    EXPECT_TRUE(code.find("int32_t process(int32_t value)") != std::string::npos);
    // Should not contain tuple
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateIntersectionTypeInReturnType) {
    std::string input = "func create() i32 & Positive { return 42; }";
    std::string code = generateCode(input);
    
    // Return type should be erased to base type
    EXPECT_TRUE(code.find("int32_t create()") != std::string::npos);
    // Should not contain tuple
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateFloatIntersectionType) {
    std::string input = "func process(value: f32 & Positive) f32 { return value; }";
    std::string code = generateCode(input);
    
    // Should erase to base float type
    EXPECT_TRUE(code.find("float process(float value)") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateStringIntersectionType) {
    std::string input = "func process(value: str & NonEmpty) str { return value; }";
    std::string code = generateCode(input);
    
    // Should erase to base string type
    EXPECT_TRUE(code.find("str process(str value)") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

// Complex Intersection Type Scenarios
TEST_F(IntersectionTypesTest, GenerateIntersectionTypeInVariableDeclaration) {
    std::string input = "func main() i32 { def value: i32 & Positive = 42; return value; }";
    std::string code = generateCode(input);
    
    // Variable should have base type
    EXPECT_TRUE(code.find("int32_t value = 42") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateIntersectionTypeInClassMember) {
    std::string input = "class Data { pub def value: i32 & Positive; }";
    std::string code = generateCode(input);
    
    // Class member should have base type
    EXPECT_TRUE(code.find("public:int32_t value;") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateGenericIntersectionType) {
    std::string input = "func<T: Number> process(value: T & Positive) T { return value; }";
    std::string code = generateCode(input);
    
    // Should erase to generic base type
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T process(T value)") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

// Mixed Type Scenarios
TEST_F(IntersectionTypesTest, GenerateMixedParameterTypes) {
    std::string input = "func process(a: i32, b: i32 & Positive, c: str) i32 { return a + b; }";
    std::string code = generateCode(input);
    
    // Should have correct parameter types
    EXPECT_TRUE(code.find("int32_t process(int32_t a,int32_t b,str c)") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateIntersectionTypeWithConstraints) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        func<T: Number> process(value: T & Positive) T {
            return value * 2;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate constraint and erased function signature
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T process(T value)") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

// Array and Complex Types
TEST_F(IntersectionTypesTest, GenerateIntersectionTypeWithArray) {
    std::string input = "func process(arr: i32[] & NonEmpty) i32 { return arr[0]; }";
    std::string code = generateCode(input);
    
    // Should erase to base array type
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
    // Base array type should be preserved
    EXPECT_TRUE(code.find("process(") != std::string::npos);
}

TEST_F(IntersectionTypesTest, GenerateIntersectionTypeWithPointer) {
    std::string input = "func process(ptr: *i32 & NotNull) i32 { return *ptr; }";
    std::string code = generateCode(input);
    
    // Should erase to base pointer type
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
    EXPECT_TRUE(code.find("process(") != std::string::npos);
}

// Error Cases and Edge Cases
TEST_F(IntersectionTypesTest, GenerateEmptyIntersectionType) {
    // This is a theoretical edge case - empty intersection should fallback gracefully
    std::string input = "func test() void {}"; // Valid fallback case
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("void test()") != std::string::npos);
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
}

// Comprehensive Integration Test
TEST_F(IntersectionTypesTest, GenerateComprehensiveIntersectionTypes) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        constraint Even<T: Number> = T % 2 == 0;
        
        class Container<T: Type> {
            pub def value: T & Positive;
        }
        
        func<T: Number> safeDivide(a: T & Positive, b: T & NonZero) T {
            return a / b;
        }
        
        func processEven(num: i32 & Even & Positive) i32 {
            return num * 2;
        }
    )";
    std::string code = generateCode(input);
    
    // All intersection types should be erased
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
    EXPECT_TRUE(code.find("std::tuple") == std::string::npos);
    
    // Should contain proper concepts
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Even = T % 2 == 0;") != std::string::npos);
    
    // Should contain erased function signatures
    EXPECT_TRUE(code.find("T safeDivide(T a,T b)") != std::string::npos);
    EXPECT_TRUE(code.find("int32_t processEven(int32_t num)") != std::string::npos);
    
    // Should contain erased class member
    EXPECT_TRUE(code.find("public:T value;") != std::string::npos);
}

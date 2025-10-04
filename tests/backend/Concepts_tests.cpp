#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class ConceptsTest : public ::testing::Test {
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

// Built-in Concepts Tests
TEST_F(ConceptsTest, GenerateBuiltInNumberConcept) {
    std::string input = "func<T: Number> test(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    // Should generate built-in Number concept
    EXPECT_TRUE(code.find("template<typename T>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    
    // Should use Number concept in requires clause
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateBuiltInTypeConcept) {
    std::string input = "func<T: Type> test(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    // Should generate built-in Type concept
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
    
    // Should use Type concept in requires clause
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateBothBuiltInConcepts) {
    std::string input = "func<T: Number, U: Type> test(x: T, y: U) T { return x; }";
    std::string code = generateCode(input);
    
    // Should generate both built-in concepts
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
    
    // Should use both in requires clause
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Type<U>") != std::string::npos);
}

// Custom Concept Generation Tests
TEST_F(ConceptsTest, GenerateCustomConceptFromConstraint) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    std::string code = generateCode(input);
    
    // Should generate custom concept
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    
    // Should also include built-in concepts
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateMultipleCustomConcepts) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        constraint Even<T: Number> = T % 2 == 0;
    )";
    std::string code = generateCode(input);
    
    // Should generate all custom concepts
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Even = T % 2 == 0;") != std::string::npos);
    
    // Each should have proper template declaration
    size_t positivePos = code.find("concept Positive");
    size_t templatePos = code.rfind("template<typename T> requires Number<T>", positivePos);
    EXPECT_NE(templatePos, std::string::npos);
}

// Concept Usage in Function Templates Tests
TEST_F(ConceptsTest, GenerateConceptConstraintInFunction) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        func<T: Number> square(x: T & Positive) T {
            return x * x;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate the constraint as a concept
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    
    // Function should use Number constraint (intersection type erased)
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T square(T x)") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateMultipleConceptConstraintsInFunction) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        func<T: Number, U: Number> safeDivide(a: T & Positive, b: U & NonZero) T {
            return a / b;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate both concepts
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    
    // Function should use both Number constraints
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Number<U>") != std::string::npos);
    EXPECT_TRUE(code.find("T safeDivide(T a,U b)") != std::string::npos);
}

// Concept Usage in Class Templates Tests
TEST_F(ConceptsTest, GenerateConceptConstraintInClass) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        class Container<T: Number> {
            pub def value: T & Positive;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate the concept
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    
    // Class should use Number constraint
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("class Container{") != std::string::npos);
    EXPECT_TRUE(code.find("public:T value;") != std::string::npos);
}

// Concrete Type Constraints (std::same_as) Tests
TEST_F(ConceptsTest, GenerateConcreteTypeConstraint) {
    std::string input = "func<T: i32> processInt(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    // Should use std::same_as for concrete type constraints
    EXPECT_TRUE(code.find("template<typename T> requires std::same_as<T, int32_t>") != std::string::npos);
    EXPECT_TRUE(code.find("T processInt(T x)") != std::string::npos);
    
    // Should include type_traits header
    EXPECT_TRUE(code.find("#include <type_traits>") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateMixedConceptAndConcreteConstraints) {
    std::string input = "func<T: Number, U: i32> process(a: T, b: U) T { return a; }";
    std::string code = generateCode(input);
    
    // Should use both concept and concrete type constraints
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && std::same_as<U, int32_t>") != std::string::npos);
    EXPECT_TRUE(code.find("T process(T a,U b)") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateMultipleConcreteTypeConstraints) {
    std::string input = "func<T: i32, U: f32, V: str> process(a: T, b: U, c: V) T { return a; }";
    std::string code = generateCode(input);
    
    // Should use std::same_as for all concrete types
    EXPECT_TRUE(code.find("template<typename T, typename U, typename V> requires std::same_as<T, int32_t> && std::same_as<U, float> && std::same_as<V, str>") != std::string::npos);
    EXPECT_TRUE(code.find("T process(T a,U b,V c)") != std::string::npos);
}

// Complex Constraint Expression Tests
TEST_F(ConceptsTest, GenerateComplexConceptExpression) {
    std::string input = "constraint ComplexCheck<T: Number> = T > 0 && T < 100 && T % 2 == 0;";
    std::string code = generateCode(input);
    
    // Should generate complex concept expression
    EXPECT_TRUE(code.find("concept ComplexCheck = T > 0 && T < 100 && T % 2 == 0;") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
}

TEST_F(ConceptsTest, GenerateConceptWithMultipleParameters) {
    std::string input = "constraint InRange<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;";
    std::string code = generateCode(input);
    
    // Should generate concept with multiple template parameters
    EXPECT_TRUE(code.find("template<typename T, typename Min, typename Max> requires Number<T> && std::same_as<Min, int32_t> && std::same_as<Max, int32_t>") != std::string::npos);
    EXPECT_TRUE(code.find("concept InRange = T >= Min && T <= Max;") != std::string::npos);
}

// Integration Tests
TEST_F(ConceptsTest, GenerateComprehensiveConceptUsage) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        
        func<T: Number> square(x: T & Positive) T {
            return x * x;
        }
        
        func<T: i32> processInt(x: T) T {
            return x + 1;
        }
        
        class Container<T: Type, U: Number> {
            pub def data: T;
            pub def count: U & Positive;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate custom concepts
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    
    // Should generate built-in concepts
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
    
    // Should use concepts in function templates
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires std::same_as<T, int32_t>") != std::string::npos);
    
    // Should use concepts in class templates
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Type<T> && Number<U>") != std::string::npos);
    
    // Should include necessary headers
    EXPECT_TRUE(code.find("#include <type_traits>") != std::string::npos);
}

// Error Handling and Edge Cases
TEST_F(ConceptsTest, HandleEmptyProgram) {
    std::string input = "";
    std::string code = generateCode(input);
    
    // Should still generate built-in concepts for empty program
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
}

TEST_F(ConceptsTest, HandleProgramWithoutGenerics) {
    std::string input = "func add(a: i32, b: i32) i32 { return a + b; }";
    std::string code = generateCode(input);
    
    // Should still generate built-in concepts even without generics
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
    
    // Function should be non-generic
    EXPECT_TRUE(code.find("int32_t add(int32_t a,int32_t b)") != std::string::npos);
    EXPECT_TRUE(code.find("template") == std::string::npos || 
                code.find("int32_t add") > code.find("template")); // Template should come before if present
}

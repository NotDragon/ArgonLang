#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class GenericFunctionCallsTest : public ::testing::Test {
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

// Generic Function Call Parsing Tests
TEST_F(GenericFunctionCallsTest, ParseGenericFunctionCallSingleType) {
    std::string input = "func main() i32 { def result = identity<i32>(42); return result; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    // Navigate to the function call inside the body
    auto blockNode = dynamic_cast<ArgonLang::BlockNode*>(funcNode->body.get());
    ASSERT_NE(blockNode, nullptr);
    ASSERT_GE(blockNode->body.size(), 1);
    
    auto varDeclNode = dynamic_cast<ArgonLang::VariableDeclarationNode*>(blockNode->body[0].get());
    ASSERT_NE(varDeclNode, nullptr);
    
    auto callNode = dynamic_cast<ArgonLang::FunctionCallExpressionNode*>(varDeclNode->value.get());
    ASSERT_NE(callNode, nullptr);
    
    EXPECT_EQ(callNode->genericTypeArgs.size(), 1);
    EXPECT_EQ(callNode->arguments.size(), 1);
}

TEST_F(GenericFunctionCallsTest, ParseGenericFunctionCallMultipleTypes) {
    std::string input = "func main() i32 { def result = combine<i32, str>(42, \"hello\"); return result; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    // Navigate to the function call
    auto blockNode = dynamic_cast<ArgonLang::BlockNode*>(funcNode->body.get());
    ASSERT_NE(blockNode, nullptr);
    ASSERT_GE(blockNode->body.size(), 1);
    
    auto varDeclNode = dynamic_cast<ArgonLang::VariableDeclarationNode*>(blockNode->body[0].get());
    ASSERT_NE(varDeclNode, nullptr);
    
    auto callNode = dynamic_cast<ArgonLang::FunctionCallExpressionNode*>(varDeclNode->value.get());
    ASSERT_NE(callNode, nullptr);
    
    EXPECT_EQ(callNode->genericTypeArgs.size(), 2);
    EXPECT_EQ(callNode->arguments.size(), 2);
}

TEST_F(GenericFunctionCallsTest, ParseRegularFunctionCallNoGenerics) {
    std::string input = "func main() i32 { def result = add(5, 10); return result; }";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    auto funcNode = dynamic_cast<ArgonLang::FunctionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(funcNode, nullptr);
    
    // Navigate to the function call
    auto blockNode = dynamic_cast<ArgonLang::BlockNode*>(funcNode->body.get());
    ASSERT_NE(blockNode, nullptr);
    ASSERT_GE(blockNode->body.size(), 1);
    
    auto varDeclNode = dynamic_cast<ArgonLang::VariableDeclarationNode*>(blockNode->body[0].get());
    ASSERT_NE(varDeclNode, nullptr);
    
    auto callNode = dynamic_cast<ArgonLang::FunctionCallExpressionNode*>(varDeclNode->value.get());
    ASSERT_NE(callNode, nullptr);
    
    EXPECT_EQ(callNode->genericTypeArgs.size(), 0);
    EXPECT_EQ(callNode->arguments.size(), 2);
}

// Generic Function Call Code Generation Tests
TEST_F(GenericFunctionCallsTest, GenerateGenericFunctionCallSingleType) {
    std::string input = "func main() i32 { def result = identity<i32>(42); return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("identity<int32_t>(42)") != std::string::npos);
}

TEST_F(GenericFunctionCallsTest, GenerateGenericFunctionCallMultipleTypes) {
    std::string input = "func main() i32 { def result = combine<i32, str>(42, \"hello\"); return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("combine<int32_t, str>(42, \"hello\")") != std::string::npos);
}

TEST_F(GenericFunctionCallsTest, GenerateGenericFunctionCallFloatType) {
    std::string input = "func main() i32 { def result = process<f32>(3.14); return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("process<float>(3.14)") != std::string::npos);
}

TEST_F(GenericFunctionCallsTest, GenerateRegularFunctionCall) {
    std::string input = "func main() i32 { def result = add(5, 10); return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("add(5, 10)") != std::string::npos);
    EXPECT_TRUE(code.find("add<") == std::string::npos); // Should not have generic args
}

// Mixed Generic and Regular Function Calls
TEST_F(GenericFunctionCallsTest, GenerateMixedFunctionCalls) {
    std::string input = R"(
        func main() i32 {
            def result1 = identity<i32>(42);
            def result2 = add(5, 10);
            def result3 = combine<str, i32>("hello", 123);
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("identity<int32_t>(42)") != std::string::npos);
    EXPECT_TRUE(code.find("add(5, 10)") != std::string::npos);
    EXPECT_TRUE(code.find("combine<str, int32_t>(\"hello\", 123)") != std::string::npos);
}

// Complex Generic Type Arguments
TEST_F(GenericFunctionCallsTest, GenerateComplexGenericTypes) {
    std::string input = "func main() i32 { def result = process<i32[]>(arr); return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("process<") != std::string::npos);
    EXPECT_TRUE(code.find(">(arr)") != std::string::npos);
}

// Nested Generic Function Calls
TEST_F(GenericFunctionCallsTest, GenerateNestedGenericCalls) {
    std::string input = "func main() i32 { def result = outer<i32>(inner<str>(\"test\")); return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("outer<int32_t>(inner<str>(\"test\"))") != std::string::npos);
}

// Error Handling Tests
TEST_F(GenericFunctionCallsTest, ParseErrorMissingClosingAngleBracket) {
    std::string input = "func main() i32 { def result = identity<i32(42); return 0; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(GenericFunctionCallsTest, ParseErrorMissingTypeArgument) {
    std::string input = "func main() i32 { def result = identity<>(42); return 0; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(GenericFunctionCallsTest, ParseErrorInvalidTypeArgument) {
    std::string input = "func main() i32 { def result = identity<123>(42); return 0; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

// Type Inference Tests (Regular calls without explicit types)
TEST_F(GenericFunctionCallsTest, TypeInferenceRegularCall) {
    std::string input = R"(
        func<T: Number> square(x: T) T { return x * x; }
        func main() i32 {
            def result = square(5);
            return result;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate C++ code that relies on template argument deduction
    EXPECT_TRUE(code.find("square(5)") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
}

TEST_F(GenericFunctionCallsTest, TypeInferenceMultipleArguments) {
    std::string input = R"(
        func<T: Number, U: Number> combine(a: T, b: U) T { return a; }
        func main() i32 {
            def result = combine(10, 3.14);
            return result;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate C++ code that relies on template argument deduction
    EXPECT_TRUE(code.find("combine(10, 3.14)") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Number<U>") != std::string::npos);
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class ArrayIndexingTest : public ::testing::Test {
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

// Basic Single Index Tests
TEST_F(ArrayIndexingTest, GenerateSingleIndex) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3]; def x = arr[1]; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[1]") != std::string::npos);
    EXPECT_TRUE(code.find("auto x = arr[1];") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateSingleIndexWithVariable) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3]; def i: i32 = 2; def x = arr[i]; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[i]") != std::string::npos);
    EXPECT_TRUE(code.find("auto x = arr[i];") != std::string::npos);
}

// Slice Indexing Tests
TEST_F(ArrayIndexingTest, GenerateSliceIndex) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def slice = arr[2:4]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + 2") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + (4 + 1)") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateSliceIndexWithVariables) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def start: i32 = 1; def end: i32 = 3; def slice = arr[start:end]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + start") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + (end + 1)") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateSliceIndexFromStart) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def slice = arr[0:3]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + 0") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + (3 + 1)") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateSliceIndexToEnd) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def slice = arr[2:4]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + 2") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + (4 + 1)") != std::string::npos);
}

// Multiple Indexing Tests
TEST_F(ArrayIndexingTest, GenerateMultipleIndex) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def multiple = arr[0,2,4]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{arr[0], arr[2], arr[4]}") != std::string::npos);
    EXPECT_TRUE(code.find("auto multiple = {arr[0], arr[2], arr[4]};") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateMultipleIndexWithVariables) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def i: i32 = 1; def j: i32 = 3; def multiple = arr[i,j,0]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{arr[i], arr[j], arr[0]}") != std::string::npos);
    EXPECT_TRUE(code.find("auto multiple = {arr[i], arr[j], arr[0]};") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateMultipleIndexTwoElements) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def multiple = arr[0,4]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{arr[0], arr[4]}") != std::string::npos);
    EXPECT_TRUE(code.find("auto multiple = {arr[0], arr[4]};") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateMultipleIndexManyElements) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5,6,7,8,9,10]; def multiple = arr[0,1,2,3,4]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{arr[0], arr[1], arr[2], arr[3], arr[4]}") != std::string::npos);
    EXPECT_TRUE(code.find("auto multiple = {arr[0], arr[1], arr[2], arr[3], arr[4]};") != std::string::npos);
}

// Mixed Indexing Tests
TEST_F(ArrayIndexingTest, GenerateMixedIndexingInFunction) {
    std::string input = R"(
        func testIndexing() i32 {
            def arr: vec<i32> = [10,20,30,40,50,60,70,80,90,100];
            def single = arr[5];
            def slice = arr[2:6];
            def multiple = arr[0,3,7,9];
            return single;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[5]") != std::string::npos);
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("{arr[0], arr[3], arr[7], arr[9]}") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateNestedArrayIndexing) {
    std::string input = "func main() i32 { def arr: vec<vec<i32>> = [[1,2],[3,4]]; def x = arr[0][1]; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[0][1]") != std::string::npos);
    EXPECT_TRUE(code.find("auto x = arr[0][1];") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateArrayIndexingInExpression) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def result = arr[1] + arr[3]; return result; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[1] + arr[3]") != std::string::npos);
    EXPECT_TRUE(code.find("auto result = arr[1] + arr[3];") != std::string::npos);
}

// Error Handling Tests
TEST_F(ArrayIndexingTest, ParseErrorMissingClosingBracket) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3]; def x = arr[1; return x; }";
    std::string code = generateCode(input);

    EXPECT_TRUE(code.find("ERROR") != std::string::npos);
}

TEST_F(ArrayIndexingTest, ParseErrorInvalidSliceSyntax) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3]; def x = arr[1:2:3]; return x; }";
    std::string code = generateCode(input);

    EXPECT_TRUE(code.find("ERROR") != std::string::npos);
}

// Complex Array Types Tests
TEST_F(ArrayIndexingTest, GenerateStringArrayIndexing) {
    std::string input = R"(
        func main() str {
            def words: vec<str> = ["hello", "world", "test"];
            def first = words[0];
            def slice = words[1:3];
            return first;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("words[0]") != std::string::npos);
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("words.begin() + 1") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateFloatArrayIndexing) {
    std::string input = R"(
        func main() f64 {
            def numbers: vec<f64> = [1.5, 2.5, 3.5, 4.5];
            def middle = numbers[1:3];
            def selected = numbers[0, 2, 3];
            return numbers[0];
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("numbers[0]") != std::string::npos);
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("{numbers[0], numbers[2], numbers[3]}") != std::string::npos);
}

// Performance and Edge Cases
TEST_F(ArrayIndexingTest, GenerateLargeMultipleIndex) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]; def multiple = arr[0,1,2,3,4,5,6,7,8,9]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9]}") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateSliceWithSameStartEnd) {
    std::string input = "func main() i32 { def arr: vec<i32> = [1,2,3,4,5]; def slice = arr[2:2]; return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("std::ranges::subrange") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + 2") != std::string::npos);
    EXPECT_TRUE(code.find("arr.begin() + (2 + 1)") != std::string::npos);
}

// Integration with Other Features
TEST_F(ArrayIndexingTest, GenerateIndexingWithFunctionCalls) {
    std::string input = R"(
        func getIndex() i32 { return 2; }
        func main() i32 {
            def arr: vec<i32> = [1,2,3,4,5];
            def x = arr[getIndex()];
            return x;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[getIndex()]") != std::string::npos);
    EXPECT_TRUE(code.find("auto x = arr[getIndex()];") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateIndexingInReturnStatement) {
    std::string input = "func getFirst(arr: vec<i32>) i32 { return arr[0]; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("return arr[0];") != std::string::npos);
}

TEST_F(ArrayIndexingTest, GenerateIndexingInConditional) {
    std::string input = R"(
        func main() i32 {
            def arr: vec<i32> = [1,2,3,4,5];
            if(arr[0] > 0) {
                return arr[1];
            } else {
                return arr[2];
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("arr[0] > 0") != std::string::npos);
    EXPECT_TRUE(code.find("return arr[1];") != std::string::npos);
    EXPECT_TRUE(code.find("return arr[2];") != std::string::npos);
}

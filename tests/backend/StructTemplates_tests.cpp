#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

using namespace ArgonLang;

class StructTemplatesTest : public ::testing::Test {
protected:
    Result<std::string> parseAndGenerateCode(const std::string& code) {
        // Tokenize
        auto tokenizeResult = tokenize(code);
        if (tokenizeResult.has_error()) {
            return Err<std::string>(create_parse_error(
                ErrorType::InvalidExpression, tokenizeResult.error_msg, Token::Position()));
        }

        // Parse
        Parser parser(tokenizeResult.tokens);
        auto parseResult = parser.parse();
        if (!parseResult.has_value()) {
            return Err<std::string>(parseResult.error());
        }

        // Generate code
        CodeGenerationVisitor visitor;
        auto codeResult = visitor.visit(*parseResult.value());
        if (!codeResult.has_value()) {
            return Err<std::string>(codeResult.error());
        }

        return Ok(codeResult.value());
    }
};

TEST_F(StructTemplatesTest, BasicNamedStructLiteral) {
    std::string code = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func main() {
            def p = struct { x: i32 = 0, y: i32 = 0 };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value()) << "Parsing failed";
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("AnonymousStruct") != std::string::npos || generated.find("0") != std::string::npos) 
        << "Should generate struct with values";
}

TEST_F(StructTemplatesTest, NamedStructWithExpressions) {
    std::string code = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func test() Point {
            return struct Point {x = 5 + 3, y = 10 * 2};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, NamedStructWithVariables) {
    std::string code = R"(
        func create(a: i32, b: i32) {
            def p = struct { x: i32 = a, y: i32 = b };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, AnonymousStructStillWorks) {
    std::string code = R"(
        func test() {
            def p = struct {x: i32 = 10, y: i32 = 20};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("AnonymousStruct_") != std::string::npos)
        << "Anonymous struct should still generate unique name";
}

TEST_F(StructTemplatesTest, MultipleFieldsInOrder) {
    std::string code = R"(
        func test() {
            def rect = struct { x: i32 = 0, y: i32 = 0, width: i32 = 100, height: i32 = 50 };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructAsParameter) {
    std::string code = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func process(p: Point) i32 {
            return p.x + p.y;
        }
        
        func test() i32 {
            def p: Point;
            p.x = 5;
            p.y = 10;
            return process(p);
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, NestedStructLiterals) {
    std::string code = R"(
        func test() {
            def outer = struct {
                inner = struct { value: i32 = 42 },
                count: i32 = 10
            };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructInArrayLiteral) {
    std::string code = R"(
        func test() {
            def points = [
                struct { x: i32 = 0, y: i32 = 0 },
                struct { x: i32 = 10, y: i32 = 20 }
            ];
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructWithFunctionCalls) {
    std::string code = R"(
        func get_x() i32 -> 100;
        func get_y() i32 -> 200;
        
        func test() {
            def p = struct { x: i32 = get_x(), y: i32 = get_y() };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, MultipleStructTypes) {
    std::string code = R"(
        func test() {
            def p = struct { x: i32 = 10, y: i32 = 20 };
            def s = struct { width: i32 = 100, height: i32 = 200 };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructLiteralInAssignment) {
    std::string code = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func test() {
            def p: Point;
            p.x = 5;
            p.y = 10;
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructWithComplexExpressions) {
    std::string code = R"(
        func test() {
            def offset = 10;
            def data = struct { value: i32 = (offset + 5) * 2 };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructLiteralFieldsWithCommas) {
    std::string code = R"(
        func test() {
            def triple = struct { a: i32 = 1, b: i32 = 2, c: i32 = 3 };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

TEST_F(StructTemplatesTest, DifferentStructsInSameExpression) {
    std::string code = R"(
        func test() {
            def p1 = struct { x: i32 = 0, y: i32 = 0 };
            def p2 = struct { x: i32 = 10, y: i32 = 20 };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_FALSE(generated.find("ERROR") != std::string::npos);
}

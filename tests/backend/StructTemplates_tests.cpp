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
            pub def x: i32;
            pub def y: i32;
        }
        
        func main() {
            def p = struct Point {x = 0, y = 0};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value()) << "Parsing failed";
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos) 
        << "Should generate 'Point{' for named struct literal";
    EXPECT_TRUE(generated.find(".x = 0") != std::string::npos) 
        << "Should use designated initializer for x";
    EXPECT_TRUE(generated.find(".y = 0") != std::string::npos) 
        << "Should use designated initializer for y";
}

TEST_F(StructTemplatesTest, NamedStructWithExpressions) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func test() Point {
            return struct Point {x = 5 + 3, y = 10 * 2};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos);
    EXPECT_TRUE(generated.find(".x = ") != std::string::npos);
    EXPECT_TRUE(generated.find(".y = ") != std::string::npos);
}

TEST_F(StructTemplatesTest, NamedStructWithVariables) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func create(a: i32, b: i32) Point {
            return struct Point {x = a, y = b};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos);
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
        class Rectangle {
            pub def x: i32;
            pub def y: i32;
            pub def width: i32;
            pub def height: i32;
        }
        
        func test() Rectangle {
            return struct Rectangle {x = 0, y = 0, width = 100, height = 50};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Rectangle{") != std::string::npos);
    EXPECT_TRUE(generated.find(".x = 0") != std::string::npos);
    EXPECT_TRUE(generated.find(".width = 100") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructAsParameter) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func process(p: Point) i32 {
            return p.x + p.y;
        }
        
        func test() i32 {
            return process(struct Point {x = 5, y = 10});
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos);
}

TEST_F(StructTemplatesTest, NestedStructLiterals) {
    std::string code = R"(
        class Inner {
            pub def value: i32;
        }
        
        class Outer {
            pub def inner: Inner;
            pub def count: i32;
        }
        
        func test() Outer {
            return struct Outer {
                inner = struct Inner {value = 42},
                count = 10
            };
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Outer{") != std::string::npos);
    EXPECT_TRUE(generated.find("Inner{") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructInArrayLiteral) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func test() {
            def points = [
                struct Point {x = 0, y = 0},
                struct Point {x = 10, y = 20}
            ];
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    
    // Count occurrences of Point{
    size_t count = 0;
    size_t pos = 0;
    while ((pos = generated.find("Point{", pos)) != std::string::npos) {
        count++;
        pos++;
    }
    EXPECT_GE(count, 2) << "Should have at least 2 Point literals";
}

TEST_F(StructTemplatesTest, StructWithFunctionCalls) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func get_x() i32 -> 100;
        func get_y() i32 -> 200;
        
        func test() Point {
            return struct Point {x = get_x(), y = get_y()};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos);
}

TEST_F(StructTemplatesTest, MultipleStructTypes) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        class Size {
            pub def width: i32;
            pub def height: i32;
        }
        
        func test() {
            def p = struct Point {x = 10, y = 20};
            def s = struct Size {width = 100, height = 200};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos);
    EXPECT_TRUE(generated.find("Size{") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructLiteralInAssignment) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func test() {
            def p: Point;
            p = struct Point {x = 5, y = 10};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Point{") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructWithComplexExpressions) {
    std::string code = R"(
        class Data {
            pub def value: i32;
        }
        
        func test() Data {
            def offset = 10;
            return struct Data {value = (offset + 5) * 2};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Data{") != std::string::npos);
}

TEST_F(StructTemplatesTest, StructLiteralFieldsWithCommas) {
    std::string code = R"(
        class Triple {
            pub def a: i32;
            pub def b: i32;
            pub def c: i32;
        }
        
        func test() Triple {
            return struct Triple {a = 1, b = 2, c = 3};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    EXPECT_TRUE(generated.find("Triple{") != std::string::npos);
    EXPECT_TRUE(generated.find(".a = 1") != std::string::npos);
    EXPECT_TRUE(generated.find(".b = 2") != std::string::npos);
    EXPECT_TRUE(generated.find(".c = 3") != std::string::npos);
}

TEST_F(StructTemplatesTest, DifferentStructsInSameExpression) {
    std::string code = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        
        func test() {
            def p1 = struct Point {x = 0, y = 0};
            def p2 = struct Point {x = 10, y = 20};
        }
    )";
    
    auto result = parseAndGenerateCode(code);
    ASSERT_TRUE(result.has_value());
    
    std::string generated = result.value();
    
    // Should have two Point{ initializers
    size_t first = generated.find("Point{");
    ASSERT_NE(first, std::string::npos);
    size_t second = generated.find("Point{", first + 1);
    EXPECT_NE(second, std::string::npos);
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

using namespace ArgonLang;

class PatternMatchingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for each test
    }

    void TearDown() override {
        // Cleanup after each test
    }

    Result<std::unique_ptr<ProgramNode>> parseAndGenerate(const std::string& code) {
        // Tokenize
        TokenizeResult tokenizeResult = ArgonLang::tokenize(code);
        if (tokenizeResult.has_error()) {
            return Err<std::unique_ptr<ProgramNode>>(create_parse_error(
                ErrorType::InvalidExpression, tokenizeResult.error_msg, Token::Position()));
        }

        // Parse
        Parser parser(tokenizeResult.tokens);
        Result<std::unique_ptr<ProgramNode>> parseResult = parser.parse();
        if (!parseResult.has_value()) {
            return Err<std::unique_ptr<ProgramNode>>(parseResult.error());
        }

        // Generate code
        CodeGenerationVisitor visitor;
        Result<std::string> codeResult = visitor.visit(*parseResult.value());
        if (!codeResult.has_value()) {
            return Err<std::unique_ptr<ProgramNode>>(codeResult.error());
        }

        return Ok(std::move(parseResult.value()));
    }
};

TEST_F(PatternMatchingTest, BasicMatchExpression) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                1 -> "one",
                2 -> "two",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, MatchWithGuards) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                n && n > 0 -> "positive",
                n && n < 0 -> "negative",
                _ -> "zero"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, ArrayDestructuring) {
    std::string code = R"(
        func test(arr: vec<i32>) i32 {
            return arr => {
                [first, second] -> first + second,
                [single] -> single,
                [] -> 0,
                _ -> -1
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, ArrayDestructuringWithRest) {
    std::string code = R"(
        func test(arr: vec<i32>) i32 {
            return arr => {
                [first, ...rest] -> first,
                [a, b, ...rest] -> a + b,
                [] -> 0,
                _ -> -1
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, StructDestructuring) {
    std::string code = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func test(p: Point) i32 {
            return p => {
                {x, y} -> x + y,
                {x = 0, y = 0} -> 0,
                _ -> -1
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, EnumPatternMatching) {
    std::string code = R"(
        enum Option {
            Some{ value: i32 },
            None
        }
        
        func test(opt: Option) i32 {
            return opt => {
                Option::Some{ value } -> value,
                Option::None -> 0,
                _ -> -1
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, RangePattern) {
    std::string code = R"(
        func test(score: i32) str {
            return score => {
                0 to 49 -> "fail",
                50 to 59 -> "pass",
                60 to= 100 -> "good",
                _ -> "invalid"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, TypePattern) {
    std::string code = R"(
        func test(value: i32) str {
            return value => {
                n && n > 0 -> "positive",
                n && n < 0 -> "negative",
                _ -> "zero"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, LiteralPattern) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                42 -> "answer",
                0 -> "zero",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, WildcardPattern) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                1 -> "one",
                _ -> "anything else"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, ComplexGuards) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                n && n > 0 && n < 10 -> "single digit",
                n && n >= 10 && n < 100 -> "double digit",
                n && n >= 100 -> "large number",
                n && n < 0 -> "negative",
                _ -> "zero"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, NestedArrayPattern) {
    std::string code = R"(
        func test(data: vec<vec<i32>>) i32 {
            return data => {
                [[first], ...rest] -> first,
                [[a, b], ...rest] -> a + b,
                [] -> 0,
                _ -> -1
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, MultipleConditions) {
    std::string code = R"(
        func test(arr: vec<i32>) str {
            return arr => {
                [x] && x > 0 -> "single positive",
                [x] && x < 0 -> "single negative",
                [a, b] && a == b -> "equal pair",
                [a, b] && a != b -> "different pair",
                arr && arr.length > 2 -> "long array",
                [] -> "empty",
                _ -> "unknown"
            };  
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, ConstructorPattern) {
    std::string code = R"(
        enum Shape {
            Circle{ radius: f32 },
            Rectangle{ width: f32, height: f32 }
        }
        
        func test(shape: Shape) f32 {
            return shape => {
                Shape::Circle{ radius } -> 3.14159 * radius * radius,
                Shape::Rectangle{ width, height } -> width * height,
                _ -> 0.0
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, PatternInVariableDeclaration) {
    std::string code = R"(
        def [first, second] = [1, 2];
        def {x, y} = struct { x = 10, y = 20 };
        def point = struct { x = x, y = y };
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, ComplexNestedPattern) {
    std::string code = R"(
        class Address {
            pub city: str;
            pub zip: str;
        }
        
        class Person {
            pub name: str;
            pub age: i32;
            pub address: Address;
        }
        
        func test(person: Person) str {
            return person => {
                {name, age, address} && age >= 18 -> "adult",
                {name, age, address} && age < 18 -> "minor",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, PatternWithArithmetic) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                n && n % 2 == 0 && n % 3 == 0 -> "divisible by 6",
                n && n % 2 == 0 -> "even",
                n && n % 3 == 0 -> "divisible by 3",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternMatchingTest, PatternWithFunctionCall) {
    std::string code = R"(
        func is_even(n: i32) bool -> n % 2 == 0;
        
        func test(x: i32) str {
            return x => {
                n && is_even(n) -> "even",
                n && !is_even(n) -> "odd",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

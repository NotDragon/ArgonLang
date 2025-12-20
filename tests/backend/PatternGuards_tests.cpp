#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

using namespace ArgonLang;

class PatternGuardsTest : public ::testing::Test {
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

TEST_F(PatternGuardsTest, BasicGuardWithIdentifier) {
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

TEST_F(PatternGuardsTest, GuardWithArithmetic) {
    std::string code = R"(
        func test(n: i32) str {
            return n => {
                x && x % 2 == 0 -> "even",
                x && x % 3 == 0 -> "divisible by 3",
                x && x % 6 == 0 -> "divisible by 6",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithArrayLength) {
    std::string code = R"(
        func test(arr: vec<i32>) str {
            return arr => {
                a && a.length == 0 -> "empty",
                a && a.length == 1 -> "single",
                a && a.length > 1 -> "multiple",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithArrayElementCheck) {
    std::string code = R"(
        func test(arr: vec<i32>) str {
            return arr => {
                a && a.length > 0 && a[0] > 0 -> "starts positive",
                a && a.length > 0 && a[0] < 0 -> "starts negative",
                a && a.length > 0 && a[0] == 0 -> "starts zero",
                [] -> "empty",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithFunctionCall) {
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

TEST_F(PatternGuardsTest, GuardWithStringOperations) {
    std::string code = R"(
        func test(s: str) str {
            return s => {
                text && text.length == 0 -> "empty",
                text && text.length < 5 -> "short",
                text && text.length > 100 -> "long",
                _ -> "regular"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithStructFieldCheck) {
    std::string code = R"(
        class Person {
            pub name: str;
            pub age: i32;
        }
        
        func test(p: Person) str {
            return p => {
                {name, age} && age < 18 -> "minor",
                {name, age} && age >= 18 && age < 65 -> "adult",
                {name, age} && age >= 65 -> "senior",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithComplexExpressions) {
    std::string code = R"(
        func test(arr: vec<i32>) str {
            return arr => {
                a && a.length == 2 && a[0] + a[1] > 100 -> "large sum",
                a && a.length >= 3 && a[0] == a[1] && a[1] == a[2] -> "three equal",
                a && a.length > 0 && a[0] * 2 > a[a.length - 1] -> "first doubled greater",
                [] -> "empty",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithRangeChecks) {
    std::string code = R"(
        func test(score: i32) str {
            return score => {
                s && s >= 0 && s <= 49 -> "fail",
                s && s >= 50 && s <= 59 -> "pass",
                s && s >= 60 && s <= 69 -> "good",
                s && s >= 70 && s <= 79 -> "very good",
                s && s >= 80 && s <= 100 -> "excellent",
                _ -> "invalid"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithBooleanLogic) {
    std::string code = R"(
        func test(data: i32) str {
            return data => {
                n && n > 0 -> "positive",
                n && n < 0 -> "negative",
                0 -> "zero",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithNestedArrayChecks) {
    std::string code = R"(
        func test(matrix: vec<vec<i32>>) str {
            return matrix => {
                rows && rows.length == 0 -> "empty",
                rows && rows.length > 0 -> "non-empty",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithMultipleConditions) {
    std::string code = R"(
        func test(x: i32) str {
            return x => {
                n && n > 0 && n < 10 -> "single digit",
                n && n >= 10 && n < 100 -> "double digit",
                n && n >= 100 && n < 1000 -> "triple digit",
                n && n >= 1000 -> "large number",
                n && n < 0 -> "negative",
                _ -> "zero"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithArrayRestPattern) {
    std::string code = R"(
        func test(arr: vec<i32>) str {
            return arr => {
                [first, ...rest] && rest.length == 0 -> "single element",
                [first, second, ...rest] && rest.length == 0 -> "pair",
                [first, second, ...rest] && rest.length > 0 -> "multiple elements",
                [] -> "empty",
                _ -> "other"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithStructDestructuring) {
    std::string code = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func test(p: Point) str {
            return p => {
                {x, y} && x > 0 && y > 0 -> "positive quadrant",
                {x, y} && x < 0 && y > 0 -> "negative x, positive y",
                {x, y} && x == 0 && y == 0 -> "origin",
                {x, y} && x == 0 -> "on y-axis",
                {x, y} && y == 0 -> "on x-axis",
                _ -> "other position"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithEnumPattern) {
    std::string code = R"(
        enum Option {
            Some{ value: i32 },
            None
        }
        
        func test(opt: Option) str {
            return opt => {
                Option::Some{ value } && value > 0 -> "positive some",
                Option::Some{ value } && value < 0 -> "negative some",
                Option::Some{ value } && value == 0 -> "zero some",
                Option::None -> "none",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(PatternGuardsTest, GuardWithComplexNestedChecks) {
    std::string code = R"(
        func test(data: vec<vec<i32>>) str {
            return data => {
                rows && rows.length > 0 -> "non-empty",
                [] -> "empty",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class ExpressionsTest : public ::testing::Test {
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

// Ternary Operator Tests
TEST_F(ExpressionsTest, GenerateBasicTernary) {
    std::string input = "func main() i32 { def x: i32 = 5 > 3 ? 1 : 0; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("5 > 3 ? 1 : 0") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateNestedTernary) {
    std::string input = "func main() i32 { def x: i32 = 5 > 3 ? (2 > 1 ? 10 : 5) : 0; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("?") != std::string::npos);
    EXPECT_TRUE(code.find(":") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateTernaryWithFunctionCalls) {
    std::string input = R"(
        func isEven(x: i32) bool { return x % 2 == 0; }
        func main() i32 {
            def result: str = isEven(4) ? "even" : "odd";
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("isEven(4)") != std::string::npos);
    EXPECT_TRUE(code.find("?") != std::string::npos);
}

// Assignment Operators Tests
TEST_F(ExpressionsTest, GenerateCompoundAssignmentAdd) {
    std::string input = "func main() i32 { def x: i32 = 5; x += 3; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x += 3") != std::string::npos || code.find("x = x + 3") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateCompoundAssignmentSubtract) {
    std::string input = "func main() i32 { def x: i32 = 10; x -= 3; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x -= 3") != std::string::npos || code.find("x = x - 3") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateCompoundAssignmentMultiply) {
    std::string input = "func main() i32 { def x: i32 = 5; x *= 2; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x *= 2") != std::string::npos || code.find("x = x * 2") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateCompoundAssignmentDivide) {
    std::string input = "func main() i32 { def x: i32 = 10; x /= 2; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x /= 2") != std::string::npos || code.find("x = x / 2") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateCompoundAssignmentModulo) {
    std::string input = "func main() i32 { def x: i32 = 10; x %= 3; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x %= 3") != std::string::npos || code.find("x = x % 3") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateCompoundAssignmentBitwise) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 15;
            x &= 7;
            x |= 8;
            x ^= 3;
            return x;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("&=") != std::string::npos || code.find("x = x &") != std::string::npos);
}

// Lambda Expressions Tests
TEST_F(ExpressionsTest, GenerateSimpleLambda) {
    std::string input = "func main() i32 { def add = (x: i32, y: i32) -> x + y; return add(3, 5); }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto add") != std::string::npos);
    EXPECT_TRUE(code.find("add(3, 5)") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateLambdaWithBlock) {
    std::string input = R"(
        func main() i32 {
            def factorial = (n: i32) -> {
                if (n <= 1) {
                    return 1;
                }
                return n * factorial(n - 1);
            };
            return factorial(5);
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto factorial") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateLambdaCapture) {
    std::string input = R"(
        func main() i32 {
            def multiplier: i32 = 3;
            def multiply = (x: i32) -> x * multiplier;
            return multiply(5);
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto multiply") != std::string::npos);
    EXPECT_TRUE(code.find("multiply(5)") != std::string::npos);
}

// Member Access Tests
TEST_F(ExpressionsTest, GenerateMemberAccess) {
    std::string input = R"(
        class Point {
            pub def x: i32;
            pub def y: i32;
        }
        func main() i32 {
            def p: Point;
            return p.x + p.y;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("p.x") != std::string::npos);
    EXPECT_TRUE(code.find("p.y") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateChainedMemberAccess) {
    std::string input = R"(
        class Address {
            pub def city: str;
        }
        class Person {
            pub def address: Address;
        }
        func main() str {
            def person: Person;
            return person.address.city;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("person.address.city") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateMethodCall) {
    std::string input = R"(
        class Counter {
            pub def value: i32;
            pub func increment() void {
                value = value + 1;
            }
        }
        func main() void {
            def counter: Counter;
            counter.increment();
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("counter.increment()") != std::string::npos);
}

// Range Expression Tests
TEST_F(ExpressionsTest, GenerateBasicRangeExclusive) {
    std::string input = "func main() void { def range = 0 to 10; }";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateBasicRangeInclusive) {
    std::string input = "func main() void { def range = 0 to= 10; }";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateRangeWithVariables) {
    std::string input = R"(
        func main() void {
            def start: i32 = 5;
            def end: i32 = 15;
            def range = start to end;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Struct Literal Tests
TEST_F(ExpressionsTest, GenerateBasicStructLiteral) {
    std::string input = "func main() void { def point = struct { x = 10, y = 20 }; }";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateStructLiteralWithTypes) {
    std::string input = "func main() void { def point = struct { x: i32 = 10, y: i32 = 20 }; }";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateNestedStructLiteral) {
    std::string input = R"(
        func main() void {
            def person = struct {
                name = "John",
                age = 30,
                address = struct {
                    city = "NYC",
                    zip = 10001
                }
            };
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Increment/Decrement Tests
TEST_F(ExpressionsTest, GeneratePreIncrement) {
    std::string input = "func main() i32 { def x: i32 = 5; return ++x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("++x") != std::string::npos);
}

TEST_F(ExpressionsTest, GeneratePostIncrement) {
    std::string input = "func main() i32 { def x: i32 = 5; return x++; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x++") != std::string::npos);
}

TEST_F(ExpressionsTest, GeneratePreDecrement) {
    std::string input = "func main() i32 { def x: i32 = 5; return --x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("--x") != std::string::npos);
}

TEST_F(ExpressionsTest, GeneratePostDecrement) {
    std::string input = "func main() i32 { def x: i32 = 5; return x--; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("x--") != std::string::npos);
}

// Complex Expression Tests
TEST_F(ExpressionsTest, GenerateComplexArithmeticExpression) {
    std::string input = "func main() i32 { return ((5 + 3) * 2 - 4) / (8 % 3 + 1); }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("return") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateComplexLogicalExpression) {
    std::string input = R"(
        func main() bool {
            def a: bool = true;
            def b: bool = false;
            def c: bool = true;
            return (a && b) || (!c && a) || (b && !a);
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("&&") != std::string::npos);
    EXPECT_TRUE(code.find("||") != std::string::npos);
    EXPECT_TRUE(code.find("!") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateMixedExpressionWithAllOperators) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 10;
            def y: i32 = 5;
            def z: i32 = 0;
            
            z = x + y * 2 - 3;
            z += 5;
            z = z > 10 ? z * 2 : z / 2;
            
            return z;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("+") != std::string::npos);
    EXPECT_TRUE(code.find("*") != std::string::npos);
    EXPECT_TRUE(code.find("?") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Bitwise Operations Tests
TEST_F(ExpressionsTest, GenerateBitwiseAnd) {
    std::string input = "func main() i32 { return 15 & 7; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("15 & 7") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateBitwiseOr) {
    std::string input = "func main() i32 { return 8 | 4; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("8 | 4") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateBitwiseXor) {
    std::string input = "func main() i32 { return 12 ^ 5; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("12 ^ 5") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateBitwiseNot) {
    std::string input = "func main() i32 { return ~15; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("~15") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateBitShiftLeft) {
    std::string input = "func main() i32 { return 1 << 4; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("1 << 4") != std::string::npos);
}

TEST_F(ExpressionsTest, GenerateBitShiftRight) {
    std::string input = "func main() i32 { return 16 >> 2; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("16 >> 2") != std::string::npos);
}

// Parallel Expression Tests
TEST_F(ExpressionsTest, GenerateParallelExpression) {
    std::string input = R"(
        func expensiveComputation() i32 { return 42; }
        func main() i32 {
            def future = par expensiveComputation();
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("par") != std::string::npos || code.find("async") != std::string::npos);
}

// Error Cases
TEST_F(ExpressionsTest, ParseErrorInvalidTernary) {
    std::string input = "func main() i32 { def x = 5 > 3 ? 1; return x; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(ExpressionsTest, ParseErrorInvalidLambdaSyntax) {
    std::string input = "func main() i32 { def f = (x: i32) -> ; return 0; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(ExpressionsTest, ParseErrorInvalidMemberAccess) {
    std::string input = "func main() i32 { def x = 5.; return x; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

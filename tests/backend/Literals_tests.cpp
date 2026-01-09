#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class LiteralsTest : public ::testing::Test {
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

// Integer Literal Tests
TEST_F(LiteralsTest, GenerateDecimalInteger) {
    std::string input = "func main() i32 { return 42; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("42") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateNegativeInteger) {
    std::string input = "func main() i32 { return -42; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("-42") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateZeroInteger) {
    std::string input = "func main() i32 { return 0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("0") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateLargeInteger) {
    std::string input = "func main() i64 { return 9223372036854775807; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("9223372036854775807") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateHexInteger) {
    std::string input = "func main() i32 { return 0xFF; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("0xFF") != std::string::npos || code.find("255") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateBinaryInteger) {
    std::string input = "func main() i32 { return 0b1010; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("0b1010") != std::string::npos || code.find("10") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateOctalInteger) {
    std::string input = "func main() i32 { return 0o77; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("0o77") != std::string::npos || code.find("63") != std::string::npos);
}

// Float Literal Tests
TEST_F(LiteralsTest, GenerateBasicFloat) {
    std::string input = "func main() f64 { return 3.14; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("3.14") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateFloatWithExponent) {
    std::string input = "func main() f64 { return 1.5e10; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("1.5e10") != std::string::npos || code.find("1.5e+10") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateFloatWithNegativeExponent) {
    std::string input = "func main() f64 { return 2.5e-3; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("2.5e-3") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateNegativeFloat) {
    std::string input = "func main() f64 { return -3.14; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("-3.14") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateZeroFloat) {
    std::string input = "func main() f64 { return 0.0; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("0.0") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateFloat32) {
    std::string input = "func main() f32 { return 3.14; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("3.14") != std::string::npos);
    EXPECT_TRUE(code.find("F32") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateFloat64) {
    std::string input = "func main() f64 { return 3.141592653589793; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("3.141592653589793") != std::string::npos);
    EXPECT_TRUE(code.find("F64") != std::string::npos);
}

// String Literal Tests
TEST_F(LiteralsTest, GenerateBasicString) {
    std::string input = R"(func main() str { return "Hello, World!"; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("\"Hello, World!\"") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateEmptyString) {
    std::string input = R"(func main() str { return ""; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("\"\"") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateStringWithEscapeSequences) {
    std::string input = R"(func main() str { return "Line 1\nLine 2\tTabbed"; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("\\n") != std::string::npos);
    EXPECT_TRUE(code.find("\\t") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateStringWithQuotes) {
    std::string input = R"(func main() str { return "He said, \"Hello!\""; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("\\\"") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateStringWithBackslash) {
    std::string input = R"(func main() str { return "C:\\path\\to\\file"; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("\\\\") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateLongString) {
    std::string input = R"(func main() str { return "This is a very long string that contains many characters and should be handled correctly by the compiler without any issues or truncation."; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("This is a very long string") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateUnicodeString) {
    std::string input = R"(func main() str { return "Hello 世界 🌍"; })";
    std::string code = generateCode(input);
    
    // Unicode should be preserved or converted
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Character Literal Tests
TEST_F(LiteralsTest, GenerateBasicChar) {
    std::string input = R"(func main() char { return 'A'; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("'A'") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateCharWithEscape) {
    std::string input = R"(func main() char { return '\n'; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("'\\n'") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateCharDigit) {
    std::string input = R"(func main() char { return '5'; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("'5'") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateCharSpecial) {
    std::string input = R"(func main() char { return '!'; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("'!'") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateCharTab) {
    std::string input = R"(func main() char { return '\t'; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("'\\t'") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateCharSingleQuote) {
    std::string input = R"(func main() char { return '\''; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("'\\''") != std::string::npos);
}

// Boolean Literal Tests
TEST_F(LiteralsTest, GenerateTrueLiteral) {
    std::string input = "func main() bool { return true; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("true") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateFalseLiteral) {
    std::string input = "func main() bool { return false; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("false") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateBooleanExpression) {
    std::string input = "func main() bool { return true && false || true; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("true") != std::string::npos);
    EXPECT_TRUE(code.find("false") != std::string::npos);
}

// Null Literal Tests
TEST_F(LiteralsTest, GenerateNullLiteral) {
    std::string input = "func main() *i32 { return null; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("null") != std::string::npos || code.find("nullptr") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Array Literal Tests
TEST_F(LiteralsTest, GenerateIntegerArrayLiteral) {
    std::string input = "func main() void { def arr: vec<i32> = [1, 2, 3, 4, 5]; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{1,2,3,4,5}") != std::string::npos || code.find("[1,2,3,4,5]") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateFloatArrayLiteral) {
    std::string input = "func main() void { def arr: vec<f64> = [1.1, 2.2, 3.3]; }";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateStringArrayLiteral) {
    std::string input = R"(func main() void { def arr: vec<str> = ["hello", "world", "test"]; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("\"hello\"") != std::string::npos);
    EXPECT_TRUE(code.find("\"world\"") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateEmptyArrayLiteral) {
    std::string input = "func main() void { def arr: vec<i32> = []; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("{}") != std::string::npos || code.find("[]") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateNestedArrayLiteral) {
    std::string input = "func main() void { def matrix: vec<vec<i32>> = [[1, 2], [3, 4], [5, 6]]; }";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Mixed Literal Types
TEST_F(LiteralsTest, GenerateMixedLiteralsInExpression) {
    std::string input = R"(
        func main() void {
            def x: i32 = 42;
            def y: f64 = 3.14;
            def msg: str = "Result";
            def flag: bool = true;
            def ptr: *i32 = null;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("42") != std::string::npos);
    EXPECT_TRUE(code.find("3.14") != std::string::npos);
    EXPECT_TRUE(code.find("\"Result\"") != std::string::npos);
    EXPECT_TRUE(code.find("true") != std::string::npos);
    EXPECT_TRUE(code.find("null") != std::string::npos || code.find("nullptr") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateComplexLiteralInitialization) {
    std::string input = R"(
        func main() void {
            def data = struct {
                count: i32 = 100,
                rate: f64 = 0.95,
                name: str = "test",
                active: bool = true,
                values: vec<i32> = [1, 2, 3, 4, 5]
            };
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("100") != std::string::npos);
    EXPECT_TRUE(code.find("0.95") != std::string::npos);
    EXPECT_TRUE(code.find("\"test\"") != std::string::npos);
    EXPECT_TRUE(code.find("true") != std::string::npos);
}

// Literal in Different Contexts
TEST_F(LiteralsTest, GenerateLiteralInFunctionArgument) {
    std::string input = R"(
        func process(x: i32, msg: str, flag: bool) void { }
        func main() void {
            process(42, "hello", true);
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("process(42, \"hello\", true)") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateLiteralInReturnStatement) {
    std::string input = R"(
        func getNumber() i32 { return 42; }
        func getMessage() str { return "success"; }
        func getFlag() bool { return true; }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("return 42") != std::string::npos);
    EXPECT_TRUE(code.find("return \"success\"") != std::string::npos);
    EXPECT_TRUE(code.find("return true") != std::string::npos);
}

TEST_F(LiteralsTest, GenerateLiteralInConditional) {
    std::string input = R"(
        func main() i32 {
            if (5 > 3 && "test" != "" && true) {
                return 1;
            }
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("5 > 3") != std::string::npos);
    EXPECT_TRUE(code.find("\"test\"") != std::string::npos);
    EXPECT_TRUE(code.find("true") != std::string::npos);
}

// Edge Cases and Error Handling
TEST_F(LiteralsTest, ParseErrorInvalidIntegerFormat) {
    std::string input = "func main() i32 { return 12.34.56; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(LiteralsTest, ParseErrorUnterminatedString) {
    std::string input = R"(func main() str { return "unterminated; })";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(LiteralsTest, ParseErrorInvalidCharLiteral) {
    std::string input = R"(func main() char { return 'AB'; })";
    auto result = parseCode(input);
    
    // Should fail - char can only be single character
    EXPECT_FALSE(result.has_value());
}

TEST_F(LiteralsTest, ParseErrorEmptyCharLiteral) {
    std::string input = R"(func main() char { return ''; })";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

// Type Inference with Literals
TEST_F(LiteralsTest, TypeInferenceInteger) {
    std::string input = "func main() void { def x = 42; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto x = 42") != std::string::npos || code.find("int") != std::string::npos);
}

TEST_F(LiteralsTest, TypeInferenceFloat) {
    std::string input = "func main() void { def y = 3.14; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto y = 3.14") != std::string::npos || code.find("ArgonLang::Runtime::f64") != std::string::npos);
}

TEST_F(LiteralsTest, TypeInferenceString) {
    std::string input = R"(func main() void { def s = "hello"; })";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto s = \"hello\"") != std::string::npos || code.find("str") != std::string::npos);
}

TEST_F(LiteralsTest, TypeInferenceBoolean) {
    std::string input = "func main() void { def b = true; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("auto b = true") != std::string::npos || code.find("bool") != std::string::npos);
}

#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"

class CodeGenerationTest : public ::testing::Test {
protected:
    void SetUp() override {}
    
    ArgonLang::Result<std::unique_ptr<ArgonLang::ProgramNode>> parseCode(const std::string& input) {
        auto tokenizeResult = ArgonLang::tokenize(input);
        if (tokenizeResult.hasError()) {
            return { tokenizeResult.errorMsg, "", ArgonLang::Trace(ArgonLang::ASTNodeType::Program, ArgonLang::Token::Position()) };
        }
        
        ArgonLang::Parser parser(tokenizeResult.tokens);
        return parser.parse();
    }
    
    std::string generateCode(const std::string& input) {
        auto parseResult = parseCode(input);
        if (parseResult.hasError()) {
            return "PARSE_ERROR: " + parseResult.getErrorMsg();
        }
        
        ArgonLang::CodeGenerationVisitor visitor;
        auto codeResult = visitor.visit(*parseResult.getValue());
        if (codeResult.hasError()) {
            return "CODEGEN_ERROR: " + codeResult.getErrorMsg();
        }
        
        return codeResult.getValue();
    }
};

// Basic Code Generation Tests
TEST_F(CodeGenerationTest, GenerateSimpleFunction) {
    std::string input = "func add(a: i32, b: i32) i32 { return a + b; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("int32_t add(int32_t a,int32_t b)") != std::string::npos);
    EXPECT_TRUE(code.find("return a + b;") != std::string::npos);
    EXPECT_TRUE(code.find("ARGON_SCOPE_BEGIN()") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateVariableDeclaration) {
    std::string input = "func main() i32 { def x: i32 = 42; return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("int32_t x = 42;") != std::string::npos);
    EXPECT_TRUE(code.find("return x;") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateClass) {
    std::string input = "class Point { pub def x: i32; pub def y: i32; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Point{") != std::string::npos);
    EXPECT_TRUE(code.find("public:int32_t x;") != std::string::npos);
    EXPECT_TRUE(code.find("public:int32_t y;") != std::string::npos);
    EXPECT_TRUE(code.find("};") != std::string::npos);
}

// Generic Code Generation Tests
TEST_F(CodeGenerationTest, GenerateGenericFunction) {
    std::string input = "func<T: Type> identity(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T identity(T x)") != std::string::npos);
    EXPECT_TRUE(code.find("return x;") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateGenericClass) {
    std::string input = "class Container<T: Type> { pub def value: T; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
    EXPECT_TRUE(code.find("class Container{") != std::string::npos);
    EXPECT_TRUE(code.find("public:T value;") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateConstraint) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("// Constraint Positive") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
}

// Function Call Code Generation Tests
TEST_F(CodeGenerationTest, GenerateFunctionCall) {
    std::string input = "func main() i32 { def result = add(5, 10); return result; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("add(5, 10)") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateGenericFunctionCall) {
    std::string input = "func main() i32 { def result = identity<i32>(42); return result; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("identity<int32_t>(42)") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateNestedFunctionCalls) {
    std::string input = "func main() i32 { def result = outer(inner(42)); return result; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("outer(inner(42))") != std::string::npos);
}

// Type Code Generation Tests
TEST_F(CodeGenerationTest, GenerateBasicTypes) {
    std::string input = R"(
        func test(a: i8, b: i16, c: i32, d: i64, e: f32, f: f64, g: str, h: bool) void {}
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("int8_t a") != std::string::npos);
    EXPECT_TRUE(code.find("int16_t b") != std::string::npos);
    EXPECT_TRUE(code.find("int32_t c") != std::string::npos);
    EXPECT_TRUE(code.find("int64_t d") != std::string::npos);
    EXPECT_TRUE(code.find("float e") != std::string::npos);
    EXPECT_TRUE(code.find("double f") != std::string::npos);
    EXPECT_TRUE(code.find("str g") != std::string::npos);
    EXPECT_TRUE(code.find("bool h") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateArrayTypes) {
    std::string input = "func test(arr: i32[10]) void {}";
    std::string code = generateCode(input);
    
    // Array types should be generated properly
    EXPECT_TRUE(code.find("test(") != std::string::npos);
}

TEST_F(CodeGenerationTest, GeneratePointerTypes) {
    std::string input = "func test(ptr: *i32) void {}";
    std::string code = generateCode(input);
    
    // Pointer types should be generated properly
    EXPECT_TRUE(code.find("test(") != std::string::npos);
}

// Expression Code Generation Tests
TEST_F(CodeGenerationTest, GenerateBinaryExpressions) {
    std::string input = "func main() i32 { return 5 + 10 * 2 - 3 / 1; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("return 5 + 10 * 2 - 3 / 1;") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateComparisonExpressions) {
    std::string input = "func main() bool { return 5 > 3 && 2 < 4 || 1 == 1; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("return 5 > 3 && 2 < 4 || 1 == 1;") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateUnaryExpressions) {
    std::string input = "func main() i32 { def x = 5; return -x + !true; }";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("-x") != std::string::npos);
    EXPECT_TRUE(code.find("!true") != std::string::npos);
}

// Control Flow Code Generation Tests
TEST_F(CodeGenerationTest, GenerateIfStatement) {
    std::string input = R"(
        func main() i32 {
            if (5 > 3) {
                return 1;
            } else {
                return 0;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("if(5 > 3)") != std::string::npos);
    EXPECT_TRUE(code.find("else") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateWhileLoop) {
    std::string input = R"(
        func main() i32 {
            def x = 0;
            while (x < 10) {
                x = x + 1;
            }
            return x;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("while(x < 10)") != std::string::npos);
}

// Complex Integration Tests
TEST_F(CodeGenerationTest, GenerateComplexGenericFunction) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        func<T: Number, U: Type> processData(num: T & Positive, data: U) T {
            return num * 2;
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate constraint
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    
    // Should generate generic function with proper constraints
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Type<U>") != std::string::npos);
    EXPECT_TRUE(code.find("T processData(T num,U data)") != std::string::npos);
    EXPECT_TRUE(code.find("return num * 2;") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateComplexGenericClass) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        class Container<T: Type, U: Number> {
            pub def data: T;
            pub def count: U & Positive;
            
            pub func getData() T {
                return data;
            }
        }
    )";
    std::string code = generateCode(input);
    
    // Should generate constraint
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    
    // Should generate generic class
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Type<T> && Number<U>") != std::string::npos);
    EXPECT_TRUE(code.find("class Container{") != std::string::npos);
    EXPECT_TRUE(code.find("public:T data;") != std::string::npos);
    EXPECT_TRUE(code.find("public:U count;") != std::string::npos);
    EXPECT_TRUE(code.find("public:T getData()") != std::string::npos);
}

TEST_F(CodeGenerationTest, GenerateBuiltInConcepts) {
    std::string input = "func<T: Number> test(x: T) T { return x; }";
    std::string code = generateCode(input);
    
    // Should generate built-in concepts
    EXPECT_TRUE(code.find("// Built-in concepts") != std::string::npos);
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
}

// Error Handling Tests
TEST_F(CodeGenerationTest, HandleParseError) {
    std::string input = "func invalid syntax here";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("PARSE_ERROR:") != std::string::npos);
}

// Comprehensive Integration Test
TEST_F(CodeGenerationTest, GenerateComprehensiveProgram) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        
        class Vec<T: Type> {
            pub def data: T[];
            pub def size: i32;
            
            pub func getSize() i32 {
                return size;
            }
        }
        
        func<T: Number> safeDivide(a: T & Positive, b: T & NonZero) T {
            return a / b;
        }
        
        func main() i32 {
            def result1 = safeDivide<i32>(10, 2);
            def result2 = safeDivide(20, 4);
            return result1 + result2;
        }
    )";
    std::string code = generateCode(input);
    
    // Should contain all major components
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
    EXPECT_TRUE(code.find("class Vec{") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T safeDivide(T a,T b)") != std::string::npos);
    EXPECT_TRUE(code.find("safeDivide<int32_t>(10, 2)") != std::string::npos);
    EXPECT_TRUE(code.find("safeDivide(20, 4)") != std::string::npos);
    EXPECT_TRUE(code.find("int32_t main()") != std::string::npos);
    
    // Should include all necessary headers and concepts
    EXPECT_TRUE(code.find("#include <type_traits>") != std::string::npos);
    EXPECT_TRUE(code.find("concept Number = std::is_arithmetic_v<T>;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Type = true;") != std::string::npos);
}

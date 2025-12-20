#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class IntegrationTest : public ::testing::Test {
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

// Complete Generic System Integration Test
TEST_F(IntegrationTest, CompleteGenericSystemIntegration) {
    std::string input = R"(
        // Define custom constraints
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
        constraint Even<T: Number> = T % 2 == 0;
        constraint ValidIndex<T: Number> = T >= 0 && T < 1000;
        
        // Generic class with constraints
        class Container<T: Type, Size: i32> {
            pub data: T[];
            pub size: Size & Positive;
            pub capacity: i32 & Positive;
            
            pub func getSize() i32 {
                return size;
            }
            
            pub func isEmpty() bool {
                return size == 0;
            }
        }
        
        // Generic functions with various constraint combinations
        func<T: Number> square(x: T & Positive) T {
            return x * x;
        }
        
        func<T: Number, U: Number> safeDivide(numerator: T & Positive, denominator: U & NonZero) T {
            return numerator / denominator;
        }
        
        func<T: Type, U: Type, V: Type> combine(first: T, second: U, third: V) T {
            return first;
        }
        
        func<Index: i32> getElement(arr: i32[], index: Index & ValidIndex) i32 {
            return arr[index];
        }
        
        // Main function with mixed generic and regular calls
        func main() i32 {
            // Explicit generic function calls
            def result1 = square<i32>(5);
            def result2 = safeDivide<i32, f32>(10, 2.0);
            def result3 = combine<i32, str, bool>(42, "hello", true);
            def result4 = getElement<i32>(arr, 5);
            
            // Type inference calls (C++ will deduce types)
            def result5 = square(7);
            def result6 = safeDivide(20, 4);
            def result7 = combine(100, 3.14, "world");
            
            // Mixed calculations
            return result1 + result5 + result6;
        }
    )";
    
    std::string code = generateCode(input);
    
    // Verify constraints are generated as concepts
    EXPECT_TRUE(code.find("concept Positive = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero = T != 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Even = T % 2 == 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept ValidIndex = T >= 0 && T < 1000;") != std::string::npos);
    
    // Verify generic class generation
    EXPECT_TRUE(code.find("template<typename T, typename Size> requires Type<T> && std::same_as<Size, ArgonLang::Runtime::i32>") != std::string::npos);
    EXPECT_TRUE(code.find("class Container{") != std::string::npos);
    EXPECT_TRUE(code.find("public:ArgonLang::Runtime::i32 getSize()") != std::string::npos);
    
    // Verify generic function templates
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T square(T x)") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Number<T> && Number<U>") != std::string::npos);
    EXPECT_TRUE(code.find("T safeDivide(T numerator,U denominator)") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T, typename U, typename V> requires Type<T> && Type<U> && Type<V>") != std::string::npos);
    EXPECT_TRUE(code.find("T combine(T first,U second,V third)") != std::string::npos);
    
    // Verify explicit generic function calls
    EXPECT_TRUE(code.find("square<ArgonLang::Runtime::i32>(5)") != std::string::npos);
    EXPECT_TRUE(code.find("safeDivide<ArgonLang::Runtime::i32, ArgonLang::Runtime::f32>(10, 2.0)") != std::string::npos);
    EXPECT_TRUE(code.find("combine<ArgonLang::Runtime::i32, str, bool>(42, \"hello\", true)") != std::string::npos);
    
    // Verify type inference calls (no explicit types)
    EXPECT_TRUE(code.find("square(7)") != std::string::npos);
    EXPECT_TRUE(code.find("safeDivide(20, 4)") != std::string::npos);
    EXPECT_TRUE(code.find("combine(100, 3.14, \"world\")") != std::string::npos);
    
    // Verify intersection types are erased
    EXPECT_TRUE(code.find("tuple") == std::string::npos);
    EXPECT_TRUE(code.find("std::tuple") == std::string::npos);
}

// Complex Constraint System Integration Test
TEST_F(IntegrationTest, ComplexConstraintSystemIntegration) {
    std::string input = R"(
        // Multi-parameter constraints
        constraint InRange<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;
        constraint ValidEmail<T: str> = T.length > 5;
        constraint PowerOfTwo<T: Number> = T > 0 && (T & (T - 1)) == 0;
        
        // Constraints used in generic functions
        func<T: Number> processRange(value: T & InRange, min: i32, max: i32) T {
            return value + min;
        }
        
        func<Email: str> validateEmail(email: Email & ValidEmail) bool {
            return true;
        }
        
        func<T: Number> optimizeForPowerOfTwo(size: T & PowerOfTwo) T {
            return size * 2;
        }
        
        // Generic class with complex constraints
        class BoundedArray<T: Type, Size: i32> {
            pub data: T[];
            pub maxSize: Size & InRange;
            
            pub func isFull() bool {
                return size >= maxSize;
            }
        }
        
        func main() i32 {
            def result1 = processRange<i32>(50, 0, 100);
            def result2 = validateEmail<str>("test@example.com");
            def result3 = optimizeForPowerOfTwo<i32>(8);
            
            return result1 + result3;
        }
    )";
    
    std::string code = generateCode(input);
    
    // Verify multi-parameter constraint generation
    EXPECT_TRUE(code.find("template<typename T, typename Min, typename Max> requires Number<T> && std::same_as<Min, ArgonLang::Runtime::i32> && std::same_as<Max, ArgonLang::Runtime::i32>") != std::string::npos);
    EXPECT_TRUE(code.find("concept InRange = T >= Min && T <= Max;") != std::string::npos);
    
    // Verify string constraint generation
    EXPECT_TRUE(code.find("template<typename T> requires std::same_as<T, str>") != std::string::npos);
    EXPECT_TRUE(code.find("concept ValidEmail = T.length > 5;") != std::string::npos);
    
    // Verify complex expression constraint
    EXPECT_TRUE(code.find("concept PowerOfTwo = T > 0 && (T & T - 1) == 0;") != std::string::npos);
    
    // Verify function templates use constraints correctly
    EXPECT_TRUE(code.find("T processRange(T value,ArgonLang::Runtime::i32 min,ArgonLang::Runtime::i32 max)") != std::string::npos);
    EXPECT_TRUE(code.find("bool validateEmail(Email email)") != std::string::npos);
    EXPECT_TRUE(code.find("T optimizeForPowerOfTwo(T size)") != std::string::npos);
    
    // Verify explicit generic calls
    EXPECT_TRUE(code.find("processRange<ArgonLang::Runtime::i32>(50, 0, 100)") != std::string::npos);
    EXPECT_TRUE(code.find("validateEmail<str>(\"test@example.com\")") != std::string::npos);
    EXPECT_TRUE(code.find("optimizeForPowerOfTwo<ArgonLang::Runtime::i32>(8)") != std::string::npos);
}

// Mixed Generic and Non-Generic Code Integration Test
TEST_F(IntegrationTest, MixedGenericNonGenericIntegration) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        
        // Non-generic functions
        func addInts(a: i32, b: i32) i32 {
            return a + b;
        }
        
        func multiplyFloats(a: f32, b: f32) f32 {
            return a * b;
        }
        
        // Generic functions
        func<T: Number> genericAdd(a: T, b: T) T {
            return a + b;
        }
        
        func<T: Number> processPositive(value: T & Positive) T {
            return value * 2;
        }
        
        // Non-generic class
        class Point {
            pub x: i32;
            pub y: i32;
            
            pub func distance() f32 {
                return x * x + y * y;
            }
        }
        
        // Generic class
        class Pair<T: Type, U: Type> {
            pub first: T;
            pub second: U;
            
            pub func getFirst() T {
                return first;
            }
        }
        
        func main() i32 {
            // Non-generic function calls
            def result1 = addInts(5, 10);
            def result2 = multiplyFloats(2.5, 4.0);
            
            // Generic function calls with explicit types
            def result3 = genericAdd<i32>(7, 8);
            def result4 = processPositive<f32>(3.14);
            
            // Generic function calls with type inference
            def result5 = genericAdd(15, 20);
            def result6 = processPositive(42);
            
            return result1 + result3 + result5;
        }
    )";
    
    std::string code = generateCode(input);
    
    // Verify non-generic functions are generated correctly
    EXPECT_TRUE(code.find("ArgonLang::Runtime::i32 addInts(ArgonLang::Runtime::i32 a,ArgonLang::Runtime::i32 b)") != std::string::npos);
    EXPECT_TRUE(code.find("ArgonLang::Runtime::f32 multiplyFloats(ArgonLang::Runtime::f32 a,ArgonLang::Runtime::f32 b)") != std::string::npos);
    
    // Verify generic functions have templates
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T genericAdd(T a,T b)") != std::string::npos);
    EXPECT_TRUE(code.find("T processPositive(T value)") != std::string::npos);
    
    // Verify non-generic class
    EXPECT_TRUE(code.find("class Point{") != std::string::npos);
    EXPECT_TRUE(code.find("public:ArgonLang::Runtime::i32 x;") != std::string::npos);
    EXPECT_TRUE(code.find("public:ArgonLang::Runtime::f32 distance()") != std::string::npos);
    
    // Verify generic class
    EXPECT_TRUE(code.find("template<typename T, typename U> requires Type<T> && Type<U>") != std::string::npos);
    EXPECT_TRUE(code.find("class Pair{") != std::string::npos);
    EXPECT_TRUE(code.find("public:T getFirst()") != std::string::npos);
    
    // Verify function calls
    EXPECT_TRUE(code.find("addInts(5, 10)") != std::string::npos);
    EXPECT_TRUE(code.find("multiplyFloats(2.5, 4.0)") != std::string::npos);
    EXPECT_TRUE(code.find("genericAdd<ArgonLang::Runtime::i32>(7, 8)") != std::string::npos);
    EXPECT_TRUE(code.find("processPositive<ArgonLang::Runtime::f32>(3.14)") != std::string::npos);
    EXPECT_TRUE(code.find("genericAdd(15, 20)") != std::string::npos);
    EXPECT_TRUE(code.find("processPositive(42)") != std::string::npos);
}

// Error Recovery and Edge Cases Integration Test
TEST_F(IntegrationTest, ErrorRecoveryAndEdgeCases) {
    // Test valid edge cases that should work
    std::string input = R"(
        // Empty constraint (should work)
        constraint AlwaysTrue<T: Type> = true;
        
        // Single character constraint name
        constraint A<T: Number> = T > 0;
        
        // Constraint with complex boolean logic
        constraint Complex<T: Number> = T > 0 && T < 100 || T == -1;
        
        // Generic function with no constraints (just Type)
        func<T: Type> store(value: T) T {
            return value;
        }
        
        // Generic class with single member
        class Box<T: Type> {
            pub value: T;
        }
        
        func main() i32 {
            def result = store<i32>(42);
            return result;
        }
    )";
    
    std::string code = generateCode(input);
    
    // Should handle all edge cases correctly
    EXPECT_TRUE(code.find("concept AlwaysTrue = true;") != std::string::npos);
    EXPECT_TRUE(code.find("concept A = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept Complex = T > 0 && T < 100 || T == -1;") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
    EXPECT_TRUE(code.find("T store(T value)") != std::string::npos);
    EXPECT_TRUE(code.find("class Box{") != std::string::npos);
    EXPECT_TRUE(code.find("store<ArgonLang::Runtime::i32>(42)") != std::string::npos);
    
    // Should not contain any error indicators
    EXPECT_TRUE(code.find("PARSE_ERROR") == std::string::npos);
    EXPECT_TRUE(code.find("CODEGEN_ERROR") == std::string::npos);
}

// Performance and Scalability Integration Test
TEST_F(IntegrationTest, PerformanceAndScalabilityIntegration) {
    // Test with many constraints and generic functions
    std::string input = R"(
        constraint C1<T: Number> = T > 0;
        constraint C2<T: Number> = T < 100;
        constraint C3<T: Number> = T % 2 == 0;
        constraint C4<T: Number> = T != 42;
        constraint C5<T: Type> = true;
        
        func<T: Number> f1(x: T & C1) T { return x; }
        func<T: Number> f2(x: T & C2) T { return x; }
        func<T: Number> f3(x: T & C3) T { return x; }
        func<T: Number> f4(x: T & C4) T { return x; }
        func<T: Type> f5(x: T & C5) T { return x; }
        
        class G1<T: Type> { pub value: T; }
        class G2<T: Number> { pub value: T & C1; }
        class G3<T: Number, U: Type> { pub a: T & C2; pub b: U; }
        
        func main() i32 {
            def r1 = f1<i32>(5);
            def r2 = f2<i32>(10);
            def r3 = f3<i32>(20);
            def r4 = f4<i32>(30);
            def r5 = f5<str>("test");
            
            return r1 + r2 + r3 + r4;
        }
    )";
    
    std::string code = generateCode(input);
    
    // Should handle multiple constraints efficiently
    EXPECT_TRUE(code.find("concept C1 = T > 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept C2 = T < 100;") != std::string::npos);
    EXPECT_TRUE(code.find("concept C3 = T % 2 == 0;") != std::string::npos);
    EXPECT_TRUE(code.find("concept C4 = T != 42;") != std::string::npos);
    EXPECT_TRUE(code.find("concept C5 = true;") != std::string::npos);
    
    // Should generate all functions correctly
    EXPECT_TRUE(code.find("T f1(T x)") != std::string::npos);
    EXPECT_TRUE(code.find("T f2(T x)") != std::string::npos);
    EXPECT_TRUE(code.find("T f3(T x)") != std::string::npos);
    EXPECT_TRUE(code.find("T f4(T x)") != std::string::npos);
    EXPECT_TRUE(code.find("T f5(T x)") != std::string::npos);
    
    // Should generate all classes correctly
    EXPECT_TRUE(code.find("class G1{") != std::string::npos);
    EXPECT_TRUE(code.find("class G2{") != std::string::npos);
    EXPECT_TRUE(code.find("class G3{") != std::string::npos);
    
    // Should generate all function calls correctly
    EXPECT_TRUE(code.find("f1<ArgonLang::Runtime::i32>(5)") != std::string::npos);
    EXPECT_TRUE(code.find("f2<ArgonLang::Runtime::i32>(10)") != std::string::npos);
    EXPECT_TRUE(code.find("f3<ArgonLang::Runtime::i32>(20)") != std::string::npos);
    EXPECT_TRUE(code.find("f4<ArgonLang::Runtime::i32>(30)") != std::string::npos);
    EXPECT_TRUE(code.find("f5<str>(\"test\")") != std::string::npos);
    
    // Should not have any errors or performance issues
    EXPECT_TRUE(code.find("PARSE_ERROR") == std::string::npos);
    EXPECT_TRUE(code.find("CODEGEN_ERROR") == std::string::npos);
}

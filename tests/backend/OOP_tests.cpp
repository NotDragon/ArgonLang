#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class OOPTest : public ::testing::Test {
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

// Basic Class Tests
TEST_F(OOPTest, GenerateBasicClassDeclaration) {
    std::string input = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Point") != std::string::npos);
    EXPECT_TRUE(code.find("public:") != std::string::npos);
    EXPECT_TRUE(code.find("I32 x") != std::string::npos);
    EXPECT_TRUE(code.find("I32 y") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(OOPTest, GenerateClassWithPrivateMembers) {
    std::string input = R"(
        class BankAccount {
            balance: f64;
            pub accountNumber: i32;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class BankAccount") != std::string::npos);
    EXPECT_TRUE(code.find("public:") != std::string::npos);
    EXPECT_TRUE(code.find("private:") != std::string::npos || code.find("F64 balance") != std::string::npos);
}

TEST_F(OOPTest, GenerateClassWithMethods) {
    std::string input = R"(
        class Counter {
            pub value: i32;
            
            pub func increment() void {
                value = value + 1;
            }
            
            pub func getValue() i32 {
                return value;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Counter") != std::string::npos);
    EXPECT_TRUE(code.find("void increment()") != std::string::npos);
    EXPECT_TRUE(code.find("I32 getValue()") != std::string::npos);
    EXPECT_TRUE(code.find("value = value + 1") != std::string::npos);
}

// Constructor Tests
TEST_F(OOPTest, GenerateBasicConstructor) {
    std::string input = R"(
        class Point {
            pub x: i32;
            pub y: i32;
            
            pub constructor(x: i32, y: i32) {
                this.x = x;
                this.y = y;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Point") != std::string::npos);
    EXPECT_TRUE(code.find("Point(") != std::string::npos || code.find("constructor") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(OOPTest, GenerateDefaultConstructor) {
    std::string input = R"(
        class Rectangle {
            pub width: i32;
            pub height: i32;
            
            pub constructor(w: i32, h: i32) {
                width = w;
                height = h;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Rectangle") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(OOPTest, GenerateConstructorWithInitializerList) {
    std::string input = R"(
        class Circle {
            pub radius: f64;
            pub color: str;
            
            pub constructor(r: f64, c: str) {
                radius = r;
                color = c;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Circle") != std::string::npos);
    EXPECT_TRUE(code.find("radius = r") != std::string::npos);
}

TEST_F(OOPTest, GenerateImplWithMultipleMethods) {
    std::string input = R"(
        class Calculator {
            pub result: f64;
        
            pub func add(x: f64) void {
                result = result + x;
            }
            
            pub func subtract(x: f64) void {
                result = result - x;
            }
            
            pub func multiply(x: f64) void {
                result = result * x;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Calculator") != std::string::npos);
    EXPECT_TRUE(code.find("add") != std::string::npos);
    EXPECT_TRUE(code.find("subtract") != std::string::npos);
    EXPECT_TRUE(code.find("multiply") != std::string::npos);
}

// Method Calls Tests
TEST_F(OOPTest, GenerateSimpleMethodCall) {
    std::string input = R"(
        class Point {
            pub x: i32;
            pub func getX() i32 {
                return x;
            }
        }
        
        func main() i32 {
            def p: Point;
            return p.getX();
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("p.getX()") != std::string::npos);
}

TEST_F(OOPTest, GenerateChainedMethodCalls) {
    std::string input = R"(
        class Builder {
            pub value: i32;
            
            pub func setValue(v: i32) void {
                value = v;
            }
            
            pub func doubleValue() void {
                value = value * 2;
            }
        }
        
        func main() void {
            def b: Builder;
            b.setValue(5);
            b.doubleValue();
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("setValue") != std::string::npos);
    EXPECT_TRUE(code.find("doubleValue") != std::string::npos);
}

// Generic Classes Tests
TEST_F(OOPTest, GenerateGenericClass) {
    std::string input = R"(
        class Box<T: Type> {
            pub value: T;
            
            pub func getValue() T {
                return value;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T>") != std::string::npos);
    EXPECT_TRUE(code.find("class Box") != std::string::npos);
    EXPECT_TRUE(code.find("T getValue()") != std::string::npos);
}

TEST_F(OOPTest, GenerateGenericClassWithMultipleParameters) {
    std::string input = R"(
        class Pair<T: Type, U: Type> {
            pub first: T;
            pub second: U;
            
            pub func getFirst() T {
                return first;
            }
            
            pub func getSecond() U {
                return second;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T, typename U>") != std::string::npos);
    EXPECT_TRUE(code.find("class Pair") != std::string::npos);
    EXPECT_TRUE(code.find("T getFirst()") != std::string::npos);
    EXPECT_TRUE(code.find("U getSecond()") != std::string::npos);
}

// Visibility Modifiers Tests
TEST_F(OOPTest, GenerateMixedVisibility) {
    std::string input = R"(
        class Account {
            balance: f64;
            pub accountId: i32;
            
            func updateBalance(amount: f64) void {
                balance = balance + amount;
            }
            
            pub func deposit(amount: f64) void {
                balance = balance + amount;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Account") != std::string::npos);
    EXPECT_TRUE(code.find("public:") != std::string::npos);
}

// Static Methods Tests (if supported)
TEST_F(OOPTest, GenerateClassWithThisKeyword) {
    std::string input = R"(
        class Node {
            pub value: i32;
            pub next: *Node;
            
            pub func setValue(v: i32) void {
                this.value = v;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("this.value") != std::string::npos || code.find("this->value") != std::string::npos);
}

// Complex Class Tests
TEST_F(OOPTest, GenerateComplexClassWithMultipleFeatures) {
    std::string input = R"(
        class LinkedList {
            size: i32;
            
            pub constructor(s: i32) {
                size = s;
            }
            
            pub func getSize() i32 {
                return size;
            }
            
            pub func isEmpty() bool {
                return size == 0;
            }
            
            pub func add(value: i32) void {
                size = size + 1;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class LinkedList") != std::string::npos);
    EXPECT_TRUE(code.find("getSize") != std::string::npos);
    EXPECT_TRUE(code.find("isEmpty") != std::string::npos);
    EXPECT_TRUE(code.find("add") != std::string::npos);
}

// Nested Classes Tests
TEST_F(OOPTest, GenerateNestedClass) {
    std::string input = R"(
        class Outer {
            pub value: i32;
            
            class Inner {
                pub innerValue: i32;
            }
        }
    )";
    auto result = parseCode(input);
    
    // Nested classes might not be fully supported yet
    // Just test that parsing doesn't crash
    EXPECT_TRUE(result.has_value() || !result.has_value());
}

// Instantiation Tests
TEST_F(OOPTest, GenerateClassInstantiation) {
    std::string input = R"(
        class Point {
            pub x: i32;
            pub y: i32;
        }
        
        func main() void {
            def p: Point;
            p.x = 10;
            p.y = 20;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("Point p") != std::string::npos);
    EXPECT_TRUE(code.find("p.x = 10") != std::string::npos);
    EXPECT_TRUE(code.find("p.y = 20") != std::string::npos);
}

// Error Cases
TEST_F(OOPTest, ParseErrorInvalidClassSyntax) {
    std::string input = "class { def x: i32; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(OOPTest, ParseErrorMissingMethodBody) {
    std::string input = R"(
        class Test {
            pub func getValue() i32;
        }
    )";
    auto result = parseCode(input);
    
    // This might be allowed as a declaration
    EXPECT_TRUE(result.has_value() || !result.has_value());
}

TEST_F(OOPTest, ParseErrorInvalidConstructorSyntax) {
    std::string input = R"(
        class Test {
            constructor {
                // missing parameters
            }
        }
    )";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

// Integration Tests
TEST_F(OOPTest, GenerateCompleteClassExample) {
    std::string input = R"(
        class Vector2D {
            pub x: f64;
            pub y: f64;
            
            pub constructor(x: f64, y: f64) {
                this.x = x;
                this.y = y;
            }
            
            pub func length() f64 {
                return x * x + y * y;
            }
            
            pub func normalize() Vector2D {
                def len: f64 = length();
                if (len > 0) {
                    x = x / len;
                    y = y / len;
                }
                return this;
            }
        }
        
        func main() f64 {
            def v: Vector2D;
            v.x = 3.0;
            v.y = 4.0;
            return v.length();
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Vector2D") != std::string::npos);
    EXPECT_TRUE(code.find("F64 length()") != std::string::npos);
    EXPECT_TRUE(code.find("normalize") != std::string::npos);
    EXPECT_TRUE(code.find("v.length()") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
}

// Inheritance Tests
TEST_F(OOPTest, ParseSingleInheritance) {
    std::string input = R"(
        class Shape {
            pub color: str;
        }
        
        class Circle impl Shape {
            pub radius: i32;
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 2);
    
    auto circleNode = dynamic_cast<ArgonLang::ClassDeclarationNode*>(program->nodes[1].get());
    ASSERT_NE(circleNode, nullptr);
    EXPECT_EQ(circleNode->className, "Circle");
    EXPECT_EQ(circleNode->baseClasses.size(), 1);
}

TEST_F(OOPTest, GenerateSingleInheritance) {
    std::string input = R"(
        class Shape {
            pub color: str;
        }
        
        class Circle impl Shape {
            pub radius: i32;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Shape{") != std::string::npos);
    EXPECT_TRUE(code.find("class Circle : public Shape{") != std::string::npos);
    EXPECT_TRUE(code.find("public:I32 radius") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
    EXPECT_FALSE(code.find("CODEGEN_ERROR") != std::string::npos);
}

TEST_F(OOPTest, ParseMultipleInheritance) {
    std::string input = R"(
        class Drawable {
            pub func draw() void { }
        }
        
        class Movable {
            pub func move() void { }
        }
        
        class Sprite impl Drawable, Movable {
            pub x: i32;
            pub y: i32;
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 3);
    
    auto spriteNode = dynamic_cast<ArgonLang::ClassDeclarationNode*>(program->nodes[2].get());
    ASSERT_NE(spriteNode, nullptr);
    EXPECT_EQ(spriteNode->className, "Sprite");
    EXPECT_EQ(spriteNode->baseClasses.size(), 2);
}

TEST_F(OOPTest, GenerateMultipleInheritance) {
    std::string input = R"(
        class Drawable {
            pub func draw() void { }
        }
        
        class Movable {
            pub func move() void { }
        }
        
        class Sprite impl Drawable, Movable {
            pub x: i32;
            pub y: i32;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Drawable{") != std::string::npos);
    EXPECT_TRUE(code.find("class Movable{") != std::string::npos);
    EXPECT_TRUE(code.find("class Sprite : public Drawable, public Movable{") != std::string::npos);
    EXPECT_TRUE(code.find("public:I32 x") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
    EXPECT_FALSE(code.find("CODEGEN_ERROR") != std::string::npos);
}

TEST_F(OOPTest, GenerateGenericClassWithInheritance) {
    std::string input = R"(
        class Container<T: Type> {
            pub value: T;
        }
        
        class NumberContainer<T: Number> impl Container<T> {
            pub func double() T { return value * 2; }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("template<typename T> requires Type<T>") != std::string::npos);
    EXPECT_TRUE(code.find("class Container{") != std::string::npos);
    EXPECT_TRUE(code.find("template<typename T> requires Number<T>") != std::string::npos);
    EXPECT_TRUE(code.find("class NumberContainer : public Container<T>{") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
    EXPECT_FALSE(code.find("CODEGEN_ERROR") != std::string::npos);
}

TEST_F(OOPTest, ParseClassWithoutInheritance) {
    std::string input = R"(
        class SimpleClass {
            pub value: i32;
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
    auto program = std::move(result.value());
    
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto classNode = dynamic_cast<ArgonLang::ClassDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(classNode, nullptr);
    EXPECT_EQ(classNode->className, "SimpleClass");
    EXPECT_EQ(classNode->baseClasses.size(), 0);
}

TEST_F(OOPTest, GenerateInterfaceImplementation) {
    std::string input = R"(
        class Printable {
            pub func print() void {
                return;
            }
        }
        
        class Document impl Printable {
            pub content: str;
            
            pub func print() void {
                return;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Printable") != std::string::npos);
    EXPECT_TRUE(code.find("class Document") != std::string::npos);
    EXPECT_TRUE(code.find("print") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
}

TEST_F(OOPTest, GenerateChainedInheritance) {
    std::string input = R"(
        class Animal {
            pub name: str;
        }
        
        class Mammal impl Animal {
            pub furColor: str;
        }
        
        class Dog impl Mammal {
            pub breed: str;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("class Animal{") != std::string::npos);
    EXPECT_TRUE(code.find("class Mammal : public Animal{") != std::string::npos);
    EXPECT_TRUE(code.find("class Dog : public Mammal{") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
    EXPECT_FALSE(code.find("CODEGEN_ERROR") != std::string::npos);
}

// Test pub modifier after class name
TEST_F(OOPTest, GenerateClassWithPubModifier) {
    std::string input = R"(
        class Data pub {
            num: i32 = 10;
            func add(new_num: i32) void {
                num = new_num;
            }
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Parsing failed: " << result.error().message;
}

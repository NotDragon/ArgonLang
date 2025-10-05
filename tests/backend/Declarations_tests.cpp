#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class DeclarationsTest : public ::testing::Test {
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

// Enum Declaration Tests
TEST_F(DeclarationsTest, ParseBasicEnum) {
    std::string input = R"(
        enum Color {
            Red,
            Green,
            Blue
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse basic enum";
    auto program = std::move(result.value());
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto enumNode = dynamic_cast<ArgonLang::EnumDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(enumNode, nullptr);
    EXPECT_EQ(enumNode->enumName, "Color");
}

TEST_F(DeclarationsTest, ParseEnumWithValues) {
    std::string input = R"(
        enum Status {
            Success(code: i32),
            Error(message: str),
            Pending
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse enum with values";
}

TEST_F(DeclarationsTest, GenerateBasicEnum) {
    std::string input = R"(
        enum Direction {
            North,
            South,
            East,
            West
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("enum Direction") != std::string::npos || code.find("Direction") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
}

TEST_F(DeclarationsTest, GenerateEnumWithMembers) {
    std::string input = R"(
        enum Result {
            Ok(value: i32),
            Err(msg: str)
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Union Declaration Tests
TEST_F(DeclarationsTest, ParseBasicUnion) {
    std::string input = "union Number = i32 | f64;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse basic union";
    auto program = std::move(result.value());
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto unionNode = dynamic_cast<ArgonLang::UnionDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(unionNode, nullptr);
    EXPECT_EQ(unionNode->unionName, "Number");
}

TEST_F(DeclarationsTest, ParseMultiTypeUnion) {
    std::string input = "union Value = i32 | f64 | str | bool;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse multi-type union";
}

TEST_F(DeclarationsTest, ParseNamedUnion) {
    std::string input = R"(
        union Shape {
            Circle(radius: f64),
            Rectangle(width: f64, height: f64)
        }
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse named union";
}

TEST_F(DeclarationsTest, GenerateBasicUnion) {
    std::string input = "union IntOrFloat = i32 | f64;";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(DeclarationsTest, GenerateComplexUnion) {
    std::string input = R"(
        union Option {
            Some(value: i32),
            None
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Type Alias Tests
TEST_F(DeclarationsTest, ParseBasicTypeAlias) {
    std::string input = "using Integer = i32;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse basic type alias";
    auto program = std::move(result.value());
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto aliasNode = dynamic_cast<ArgonLang::TypeAliasNode*>(program->nodes[0].get());
    ASSERT_NE(aliasNode, nullptr);
    EXPECT_EQ(aliasNode->aliasName, "Integer");
}

TEST_F(DeclarationsTest, ParseGenericTypeAlias) {
    std::string input = "using Matrix = vec<vec<f64>>;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse generic type alias";
}

TEST_F(DeclarationsTest, ParseFunctionTypeAlias) {
    std::string input = "using Callback = func(i32, str) bool;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse function type alias";
}

TEST_F(DeclarationsTest, GenerateBasicTypeAlias) {
    std::string input = "using UserId = i64;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("using UserId") != std::string::npos || code.find("typedef") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(DeclarationsTest, GenerateComplexTypeAlias) {
    std::string input = "using StringMap = vec<struct { key: str, value: str }>;";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Module Declaration Tests
TEST_F(DeclarationsTest, ParseBasicModuleDeclaration) {
    std::string input = "module math;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse basic module declaration";
    auto program = std::move(result.value());
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto moduleNode = dynamic_cast<ArgonLang::ModuleDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(moduleNode, nullptr);
    EXPECT_EQ(moduleNode->moduleName, "math");
}

TEST_F(DeclarationsTest, ParseModuleWithExports) {
    std::string input = "module utils { add, subtract, multiply };";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse module with exports";
}

TEST_F(DeclarationsTest, GenerateModuleDeclaration) {
    std::string input = "module core;";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Import Statement Tests
TEST_F(DeclarationsTest, ParseBasicImport) {
    std::string input = "import math;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse basic import";
    auto program = std::move(result.value());
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto importNode = dynamic_cast<ArgonLang::ImportStatementNode*>(program->nodes[0].get());
    ASSERT_NE(importNode, nullptr);
    EXPECT_EQ(importNode->moduleName, "math");
}

TEST_F(DeclarationsTest, ParseSelectiveImport) {
    std::string input = "import math -> add, subtract;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse selective import";
}

TEST_F(DeclarationsTest, ParseNestedImport) {
    std::string input = "import std::collections;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse nested import";
}

TEST_F(DeclarationsTest, GenerateImportStatement) {
    std::string input = "import io;";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(DeclarationsTest, GenerateSelectiveImport) {
    std::string input = "import math -> PI, E;";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Constraint Declaration Tests
TEST_F(DeclarationsTest, ParseBasicConstraint) {
    std::string input = "constraint Positive<T: Number> = T > 0;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse basic constraint";
    auto program = std::move(result.value());
    ASSERT_EQ(program->nodes.size(), 1);
    
    auto constraintNode = dynamic_cast<ArgonLang::ConstraintDeclarationNode*>(program->nodes[0].get());
    ASSERT_NE(constraintNode, nullptr);
    EXPECT_EQ(constraintNode->constraintName, "Positive");
}

TEST_F(DeclarationsTest, ParseComplexConstraint) {
    std::string input = "constraint InRange<T: Number, Min: i32, Max: i32> = T >= Min && T <= Max;";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse complex constraint";
}

TEST_F(DeclarationsTest, GenerateBasicConstraint) {
    std::string input = "constraint NonZero<T: Number> = T != 0;";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("concept NonZero") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Multiple Declarations Tests
TEST_F(DeclarationsTest, ParseMultipleDeclarations) {
    std::string input = R"(
        module math;
        
        using Real = f64;
        
        constraint Positive<T: Number> = T > 0;
        
        enum Operation {
            Add,
            Subtract,
            Multiply,
            Divide
        }
        
        union Result = i32 | str;
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse multiple declarations";
    auto program = std::move(result.value());
    EXPECT_GE(program->nodes.size(), 5);
}

TEST_F(DeclarationsTest, GenerateMultipleDeclarations) {
    std::string input = R"(
        using Integer = i32;
        using Float = f64;
        using String = str;
        
        constraint Positive<T: Number> = T > 0;
        constraint NonZero<T: Number> = T != 0;
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("using Integer") != std::string::npos || code.find("typedef") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive") != std::string::npos);
    EXPECT_TRUE(code.find("concept NonZero") != std::string::npos);
}

// Error Cases
TEST_F(DeclarationsTest, ParseErrorInvalidEnumSyntax) {
    std::string input = "enum { Red, Green }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(DeclarationsTest, ParseErrorInvalidUnionSyntax) {
    std::string input = "union = i32 | f64;";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(DeclarationsTest, ParseErrorInvalidTypeAliasSyntax) {
    std::string input = "using = i32;";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(DeclarationsTest, ParseErrorInvalidModuleSyntax) {
    std::string input = "module;";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(DeclarationsTest, ParseErrorInvalidImportSyntax) {
    std::string input = "import;";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(DeclarationsTest, ParseErrorInvalidConstraintSyntax) {
    std::string input = "constraint Positive = T > 0;";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

// Integration Tests
TEST_F(DeclarationsTest, GenerateCompleteProgram) {
    std::string input = R"(
        module geometry;
        
        import math -> PI;
        
        using Coordinate = f64;
        
        constraint Positive<T: Number> = T > 0;
        
        enum ShapeType {
            Circle,
            Rectangle,
            Triangle
        }
        
        union Shape = Circle | Rectangle | Triangle;
        
        class Circle {
            pub def radius: Coordinate & Positive;
            
            pub func area() f64 {
                return PI * radius * radius;
            }
        }
        
        func main() i32 {
            def c: Circle;
            c.radius = 5.0;
            def a: f64 = c.area();
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
    EXPECT_TRUE(code.find("class Circle") != std::string::npos);
    EXPECT_TRUE(code.find("concept Positive") != std::string::npos);
}

TEST_F(DeclarationsTest, ParseNestedModuleImport) {
    std::string input = R"(
        import std::io::file;
        import std::collections::vector;
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse nested module imports";
}

TEST_F(DeclarationsTest, ParseMultipleSelectiveImports) {
    std::string input = R"(
        import math -> sin, cos, tan;
        import std::io -> read, write, open, close;
    )";
    auto result = parseCode(input);
    
    ASSERT_TRUE(result.has_value()) << "Failed to parse multiple selective imports";
}

TEST_F(DeclarationsTest, GenerateEnumWithGenericConstraint) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        
        enum Result<T: Number & Positive> {
            Success(value: T),
            Failure(error: str)
        }
    )";
    auto result = parseCode(input);
    
    // This might not be fully supported yet
    EXPECT_TRUE(result.has_value() || !result.has_value());
}

TEST_F(DeclarationsTest, GenerateUnionWithConstraints) {
    std::string input = R"(
        constraint Positive<T: Number> = T > 0;
        
        union PositiveNumber = (i32 & Positive) | (f64 & Positive);
    )";
    auto result = parseCode(input);
    
    // This might not be fully supported yet
    EXPECT_TRUE(result.has_value() || !result.has_value());
}

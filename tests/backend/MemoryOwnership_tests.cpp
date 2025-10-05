#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

using namespace ArgonLang;

class MemoryOwnershipTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for each test
    }

    void TearDown() override {
        // Cleanup after each test
    }

    Result<std::unique_ptr<ProgramNode>> parseAndGenerate(const std::string& code) {
        // Tokenize
        TokenizeResult tokenizeResult = Tokenizer::tokenize(code);
        if (tokenizeResult.hasError()) {
            return Err<std::unique_ptr<ProgramNode>>(create_parse_error(
                ErrorType::InvalidExpression, tokenizeResult.errorMsg, Token::Position()));
        }

        // Parse
        Parser parser(tokenizeResult.tokens);
        Result<std::unique_ptr<ProgramNode>> parseResult = parser.parseProgram();
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

TEST_F(MemoryOwnershipTest, RawPointerType) {
    std::string code = R"(
        func test(ptr: *i32) i32 {
            return *ptr;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnedType) {
    std::string code = R"(
        func test(owned: ~i32) i32 {
            return *owned;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, ImmutableReferenceType) {
    std::string code = R"(
        func test(ref: &i32) i32 {
            return ref;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, MutableReferenceType) {
    std::string code = R"(
        func test(mut_ref: &&i32) void {
            mut_ref = 42;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, NestedOwnershipTypes) {
    std::string code = R"(
        func test(ptr_to_owned: *~i32) ~i32 {
            return *ptr_to_owned;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, ReferenceToOwned) {
    std::string code = R"(
        func test(ref_to_owned: &~i32) i32 {
            return *(*ref_to_owned);
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, ArrayWithOwnership) {
    std::string code = R"(
        func test(owned_array: ~vec<i32>) i32 {
            return owned_array.length;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, GenericOwnership) {
    std::string code = R"(
        func test<T>(owned: ~T) T {
            return *owned;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, MultipleOwnershipParameters) {
    std::string code = R"(
        func test(owned1: ~i32, owned2: ~i32, ref1: &i32, ref2: &&i32) i32 {
            return *owned1 + *owned2 + ref1 + ref2;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithStructs) {
    std::string code = R"(
        struct Node {
            value: i32,
            next: ~Node
        }
        
        func test(node: ~Node) i32 {
            return node.value;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithFunctionTypes) {
    std::string code = R"(
        func test(func_ptr: *func(i32) i32) i32 {
            return (*func_ptr)(42);
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, ComplexOwnershipChain) {
    std::string code = R"(
        func test(ptr_to_ref: *&i32) i32 {
            return **ptr_to_ref;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithVariadicTypes) {
    std::string code = R"(
        func test(...owned: ~i32) i32 {
            return 0;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithIntersectionTypes) {
    std::string code = R"(
        func test(value: ~(i32 & Number)) i32 {
            return *value;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithSumTypes) {
    std::string code = R"(
        func test(value: ~(i32 | str)) str {
            return "test";
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipReturnTypes) {
    std::string code = R"(
        func create_owned() ~i32 {
            return new i32(42);
        }
        
        func get_reference() &i32 {
            let owned = new i32(42);
            return &(*owned);
        }
        
        func get_mutable_reference() &&i32 {
            let owned = new i32(42);
            return &(*owned);
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithConstraints) {
    std::string code = R"(
        func test<T: Number>(owned: ~T) T {
            return *owned;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipInClassMembers) {
    std::string code = R"(
        class TestClass {
            owned_member: ~i32,
            ref_member: &i32,
            mut_ref_member: &&i32,
            ptr_member: *i32
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithLambda) {
    std::string code = R"(
        func test() void {
            let owned = new i32(42);
            let lambda = x -> *owned + x;
            print(lambda(10));
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithPatternMatching) {
    std::string code = R"(
        func test(value: ~(i32 | str)) str {
            value => {
                ~i32(x) -> "integer: " + x,
                ~str(s) -> "string: " + s,
                _ -> "unknown"
            }
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipAssignment) {
    std::string code = R"(
        func test() void {
            let owned1: ~i32 = new i32(42);
            let owned2: ~i32 = owned1; // Move semantics
            let ref: &i32 = &(*owned2);
            let mut_ref: &&i32 = &(*owned2);
            mut_ref = 100;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

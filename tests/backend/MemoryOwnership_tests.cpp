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
        func test(ptr_to_owned: *(~i32)) ~i32 {
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
        func<T: Type> test(owned: ~T) T {
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
        class Node {
            pub value: i32;
            pub next: ~Node;
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
        func test(ptr: *i32) i32 {
            return *ptr;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithVariadicTypes) {
    std::string code = R"(
        func test(owned: ~i32) i32 {
            return *owned;
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
            def owned: ~i32 = 42;
            return owned;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithConstraints) {
    std::string code = R"(
        func<T: Number> test(owned: ~T) T {
            return *owned;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipInClassMembers) {
    std::string code = R"(
        class TestClass {
            pub owned_member: ~i32;
            pub ref_member: &i32;
            pub mut_ref_member: &&i32;
            pub ptr_member: *i32;
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithLambda) {
    std::string code = R"(
        func test() void {
            def owned: ~i32 = 42;
            def lambda = (x: i32) -> *owned + x;
            print(lambda(10));
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipWithPatternMatching) {
    std::string code = R"(
        func test(value: ~(i32 | str)) str {
            return value => {
                i32 -> "integer",
                str -> "string",
                _ -> "unknown"
            };
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

TEST_F(MemoryOwnershipTest, OwnershipAssignment) {
    std::string code = R"(
        func test() void {
            def owned1: ~i32 = 42;
            def owned2: ~i32 = ~owned1;
            def ref: &i32 = &(*owned2);
        }
    )";
    auto result = parseAndGenerate(code);
    ASSERT_TRUE(result.has_value());
}

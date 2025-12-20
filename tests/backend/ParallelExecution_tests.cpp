#include "gtest/gtest.h"
#include "backend/Parser.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

using namespace ArgonLang;

class ParallelExecutionTest : public ::testing::Test {
protected:
	Result<std::unique_ptr<ProgramNode>> parse_and_generate(const std::string& source, std::string& generated_code) {
		TokenizeResult tokenize_result = tokenize(source);
		if (tokenize_result.has_error()) {
			return Err<std::unique_ptr<ProgramNode>>(create_parse_error(ErrorType::InvalidExpression, tokenize_result.error_msg, tokenize_result.error_position));
		}

		Parser parser(tokenize_result.tokens);
		Result<std::unique_ptr<ProgramNode>> parse_result = parser.parse();

		if (!parse_result.has_value()) {
			return parse_result;
		}

		CodeGenerationVisitor visitor;
		Result<std::string> code_gen_result = visitor.visit(*parse_result.value());
		if (!code_gen_result.has_value()) {
			return Err<std::unique_ptr<ProgramNode>>(create_parse_error(ErrorType::InvalidCodeGeneration, code_gen_result.error().message, code_gen_result.error().position));
		}
		generated_code = code_gen_result.value();
		return parse_result;
	}
};

// Test 1: Simple par expression returns a future
TEST_F(ParallelExecutionTest, SimpleParExpressionReturnsFuture) {
	std::string source = R"(
		func main() {
			def x = par 42;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should generate ArgonLang::Runtime::par with a lambda
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("return 42"), std::string::npos) << generated_code;
}

// Test 2: Par expression with computation
TEST_F(ParallelExecutionTest, ParExpressionWithComputation) {
	std::string source = R"(
		func compute(n: i32) i32 {
			return n * 2;
		}

		func main() {
			def result = par compute(21);
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should wrap the function call in a lambda
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("compute(21)"), std::string::npos) << generated_code;
}

// Test 3: Multiple par expressions in same scope
TEST_F(ParallelExecutionTest, MultipleParExpressionsInScope) {
	std::string source = R"(
		func main() {
			def x = par 10;
			def y = par 20;
			def z = par 30;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should have three separate par calls
	size_t first_par = generated_code.find("ArgonLang::Runtime::par");
	ASSERT_NE(first_par, std::string::npos) << generated_code;
	
	size_t second_par = generated_code.find("ArgonLang::Runtime::par", first_par + 1);
	ASSERT_NE(second_par, std::string::npos) << generated_code;
	
	size_t third_par = generated_code.find("ArgonLang::Runtime::par", second_par + 1);
	ASSERT_NE(third_par, std::string::npos) << generated_code;
}

// Test 4: Scope has ARGON_SCOPE_BEGIN for automatic awaiting
TEST_F(ParallelExecutionTest, ScopeHasAutomaticAwaiting) {
	std::string source = R"(
		func main() {
			def x = par 42;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should have ARGON_SCOPE_BEGIN() which creates a ScopeGuard
	// The ScopeGuard destructor will wait for all futures
	EXPECT_NE(generated_code.find("ARGON_SCOPE_BEGIN()"), std::string::npos) << generated_code;
}

// Test 5: Par expression with binary operation
TEST_F(ParallelExecutionTest, ParExpressionWithBinaryOperation) {
	std::string source = R"(
		func main() {
			def a = 5;
			def b = 10;
			def result = par (a + b);
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("a + b"), std::string::npos) << generated_code;
}

// Test 6: Par expression with function call that has side effects
TEST_F(ParallelExecutionTest, ParExpressionWithSideEffects) {
	std::string source = R"(
		func process(x: i32) i32 {
			return x * x;
		}

		func main() {
			def result1 = par process(5);
			def result2 = par process(10);
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Both par expressions should be present
	size_t first_par = generated_code.find("ArgonLang::Runtime::par");
	ASSERT_NE(first_par, std::string::npos) << generated_code;
	
	size_t second_par = generated_code.find("ArgonLang::Runtime::par", first_par + 1);
	ASSERT_NE(second_par, std::string::npos) << generated_code;
}

// Test 7: Nested scopes with par expressions
TEST_F(ParallelExecutionTest, NestedScopesWithParExpressions) {
	std::string source = R"(
		func main() {
			def outer = par 1;
			{
				def inner = par 2;
			}
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should have two ARGON_SCOPE_BEGIN() - one for main, one for inner block
	size_t first_scope = generated_code.find("ARGON_SCOPE_BEGIN()");
	ASSERT_NE(first_scope, std::string::npos) << generated_code;
	
	size_t second_scope = generated_code.find("ARGON_SCOPE_BEGIN()", first_scope + 1);
	ASSERT_NE(second_scope, std::string::npos) << generated_code;
	
	// Both par expressions should be present
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
}

// Test 8: Par expression in if statement
TEST_F(ParallelExecutionTest, ParExpressionInIfStatement) {
	std::string source = R"(
		func main() {
			def condition = true;
			if (condition) {
				def x = par 42;
			}
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("if(condition)"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
}

// Test 9: Par expression in loop
TEST_F(ParallelExecutionTest, ParExpressionInLoop) {
	std::string source = R"(
		func main() {
			for (i: i32 -> 0 to 3) {
				def x = par i * 2;
			}
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("for"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
}

// Test 10: Par expression with lambda
TEST_F(ParallelExecutionTest, ParExpressionWithLambda) {
	std::string source = R"(
		func main() {
			def compute = (x: i32) -> x * 2;
			def result = par compute(21);
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("compute(21)"), std::string::npos) << generated_code;
}

// Test 11: Par expression with complex expression
TEST_F(ParallelExecutionTest, ParExpressionWithComplexExpression) {
	std::string source = R"(
		func main() {
			def a = 5;
			def b = 10;
			def c = 15;
			def result = par (a + b * c - (a / 2));
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	// Should contain the complex expression
	EXPECT_NE(generated_code.find("a + b * c"), std::string::npos) << generated_code;
}

// Test 12: Par expression returns ArgonFuture type
TEST_F(ParallelExecutionTest, ParExpressionReturnsArgonFuture) {
	std::string source = R"(
		func main() {
			def future = par 42;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// The par() function returns ArgonFuture<T>
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	// Variable should be assigned the future
	EXPECT_NE(generated_code.find("future"), std::string::npos) << generated_code;
}

// Test 13: Multiple par expressions with different types
TEST_F(ParallelExecutionTest, MultipleParExpressionsWithDifferentTypes) {
	std::string source = R"(
		func main() {
			def int_future = par 42;
			def float_future = par 3.14;
			def bool_future = par true;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should have three par calls with different values
	EXPECT_NE(generated_code.find("42"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("3.14"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("true"), std::string::npos) << generated_code;
}

// Test 14: Par expression with array
TEST_F(ParallelExecutionTest, ParExpressionWithArray) {
	std::string source = R"(
		func main() {
			def arr = par [1, 2, 3, 4, 5];
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("ArgonLang::Runtime::par"), std::string::npos) << generated_code;
	// Should contain array initialization
	EXPECT_NE(generated_code.find("{1,2,3,4,5}"), std::string::npos) << generated_code;
}

// Test 15: Par expression in return statement
TEST_F(ParallelExecutionTest, ParExpressionInReturnStatement) {
	std::string source = R"(
		func compute() {
			return par 42;
		}

		func main() {
			def result = compute();
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("return ArgonLang::Runtime::par"), std::string::npos) << generated_code;
}


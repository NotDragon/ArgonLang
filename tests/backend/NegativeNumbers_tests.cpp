#include "gtest/gtest.h"
#include "backend/Parser.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

using namespace ArgonLang;

class NegativeNumbersTest : public ::testing::Test {
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

// Test 1: Simple negative integer
TEST_F(NegativeNumbersTest, SimpleNegativeInteger) {
	std::string source = R"(
		func main() {
			def x = -1;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should generate -1 directly, not as a unary minus operation
	EXPECT_NE(generated_code.find("-1"), std::string::npos) << generated_code;
}

// Test 2: Negative float
TEST_F(NegativeNumbersTest, NegativeFloat) {
	std::string source = R"(
		func main() {
			def x = -3.14;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-3.14"), std::string::npos) << generated_code;
}

// Test 3: Multiple negative numbers
TEST_F(NegativeNumbersTest, MultipleNegativeNumbers) {
	std::string source = R"(
		func main() {
			def a = -10;
			def b = -20;
			def c = -30;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-10"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-20"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-30"), std::string::npos) << generated_code;
}

// Test 4: Negative number in arithmetic expression
TEST_F(NegativeNumbersTest, NegativeNumberInArithmetic) {
	std::string source = R"(
		func main() {
			def result = 5 + -3;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("5 + -3"), std::string::npos) << generated_code;
}

// Test 5: Negative number in array
TEST_F(NegativeNumbersTest, NegativeNumberInArray) {
	std::string source = R"(
		func main() {
			def arr = [-1, -2, -3, 4, 5];
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-1"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-2"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-3"), std::string::npos) << generated_code;
}

// Test 6: Negative number as function argument
TEST_F(NegativeNumbersTest, NegativeNumberAsFunctionArgument) {
	std::string source = R"(
		func add(a: i32, b: i32) -> i32 {
			return a + b;
		}

		func main() {
			def result = add(-5, 10);
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("add(-5, 10)"), std::string::npos) << generated_code;
}

// Test 7: Negative number in return statement
TEST_F(NegativeNumbersTest, NegativeNumberInReturnStatement) {
	std::string source = R"(
		func get_negative() -> i32 {
			return -42;
		}

		func main() {
			def x = get_negative();
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("return -42"), std::string::npos) << generated_code;
}

// Test 8: Negative number in comparison
TEST_F(NegativeNumbersTest, NegativeNumberInComparison) {
	std::string source = R"(
		func main() {
			def x = 5;
			if (x > -10) {
				def y = 1;
			}
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("x > -10"), std::string::npos) << generated_code;
}

// Test 9: Very large negative number
TEST_F(NegativeNumbersTest, LargeNegativeNumber) {
	std::string source = R"(
		func main() {
			def x = -999999;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-999999"), std::string::npos) << generated_code;
}

// Test 10: Negative zero
TEST_F(NegativeNumbersTest, NegativeZero) {
	std::string source = R"(
		func main() {
			def x = -0;
			def y = -0.0;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Both should parse successfully
	EXPECT_TRUE(generated_code.find("x") != std::string::npos) << generated_code;
	EXPECT_TRUE(generated_code.find("y") != std::string::npos) << generated_code;
}

// Test 11: Negative number in ternary expression
TEST_F(NegativeNumbersTest, NegativeNumberInTernary) {
	std::string source = R"(
		func main() {
			def result = true ? -5 : -10;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-5"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-10"), std::string::npos) << generated_code;
}

// Test 12: Negative number in loop
TEST_F(NegativeNumbersTest, NegativeNumberInLoop) {
	std::string source = R"(
		func main() {
			for (def i = -5; i < 5; i = i + 1) {
				def x = i;
			}
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-5"), std::string::npos) << generated_code;
}

// Test 13: Negative number with multiplication
TEST_F(NegativeNumbersTest, NegativeNumberWithMultiplication) {
	std::string source = R"(
		func main() {
			def result = -5 * 3;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-5"), std::string::npos) << generated_code;
}

// Test 14: Negative number in parentheses
TEST_F(NegativeNumbersTest, NegativeNumberInParentheses) {
	std::string source = R"(
		func main() {
			def result = (-5) + 10;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-5"), std::string::npos) << generated_code;
}

// Test 15: Mixed positive and negative numbers
TEST_F(NegativeNumbersTest, MixedPositiveAndNegative) {
	std::string source = R"(
		func main() {
			def a = 10;
			def b = -10;
			def c = 20;
			def d = -20;
			def result = a + b + c + d;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("10"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-10"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("20"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-20"), std::string::npos) << generated_code;
}

// Test 16: Negative float with many decimal places
TEST_F(NegativeNumbersTest, NegativeFloatManyDecimals) {
	std::string source = R"(
		func main() {
			def x = -3.141592653589793;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-3.141592"), std::string::npos) << generated_code;
}

// Test 17: Subtraction should still work (not confused with negative literal)
TEST_F(NegativeNumbersTest, SubtractionStillWorks) {
	std::string source = R"(
		func main() {
			def a = 10;
			def b = 5;
			def result = a - b;
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	// Should generate subtraction, not negative literal
	EXPECT_NE(generated_code.find("a - b"), std::string::npos) << generated_code;
}

// Test 18: Negative number in match expression
TEST_F(NegativeNumbersTest, NegativeNumberInMatch) {
	std::string source = R"(
		func main() {
			def x = -5;
			def result = match x {
				-5 -> 1,
				-10 -> 2,
				_ -> 0
			};
		}
	)";
	std::string generated_code;
	Result<std::unique_ptr<ProgramNode>> result = parse_and_generate(source, generated_code);

	ASSERT_TRUE(result.has_value()) << result.error().message;
	
	EXPECT_NE(generated_code.find("-5"), std::string::npos) << generated_code;
	EXPECT_NE(generated_code.find("-10"), std::string::npos) << generated_code;
}


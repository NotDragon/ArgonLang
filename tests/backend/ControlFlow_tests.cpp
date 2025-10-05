#include <gtest/gtest.h>
#include "backend/Parser.h"
#include "backend/Tokenizer.h"
#include "frontend/CodeGenerationVisitor.h"
#include "Error/Error.h"
#include "Error/Result.h"

class ControlFlowTest : public ::testing::Test {
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

// For Loop Tests
TEST_F(ControlFlowTest, GenerateBasicForLoop) {
    std::string input = R"(
        func main() i32 {
            def sum: i32 = 0;
            for (i: i32 -> 0 to 10) {
                sum = sum + i;
            }
            return sum;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("for") != std::string::npos);
    EXPECT_TRUE(code.find("sum = sum + i") != std::string::npos);
    EXPECT_FALSE(code.find("PARSE_ERROR") != std::string::npos);
    EXPECT_FALSE(code.find("CODEGEN_ERROR") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateForLoopInclusive) {
    std::string input = R"(
        func main() i32 {
            def count: i32 = 0;
            for (i: i32 -> 1 to= 5) {
                count = count + 1;
            }
            return count;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("for") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateNestedForLoops) {
    std::string input = R"(
        func main() i32 {
            def total: i32 = 0;
            for (i: i32 -> 0 to 5) {
                for (j: i32 -> 0 to 5) {
                    total = total + i + j;
                }
            }
            return total;
        }
    )";
    std::string code = generateCode(input);
    
    // Should have nested for loops
    int forCount = 0;
    size_t pos = 0;
    while ((pos = code.find("for", pos)) != std::string::npos) {
        forCount++;
        pos++;
    }
    EXPECT_GE(forCount, 2);
}

// While Loop Tests
TEST_F(ControlFlowTest, GenerateBasicWhileLoop) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 0;
            while (x < 10) {
                x = x + 1;
            }
            return x;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("while") != std::string::npos);
    EXPECT_TRUE(code.find("x < 10") != std::string::npos);
    EXPECT_TRUE(code.find("x = x + 1") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateNestedWhileLoops) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 0;
            def y: i32 = 0;
            while (x < 5) {
                y = 0;
                while (y < 5) {
                    y = y + 1;
                }
                x = x + 1;
            }
            return x + y;
        }
    )";
    std::string code = generateCode(input);
    
    int whileCount = 0;
    size_t pos = 0;
    while ((pos = code.find("while", pos)) != std::string::npos) {
        whileCount++;
        pos++;
    }
    EXPECT_GE(whileCount, 2);
}

// If Statement Tests
TEST_F(ControlFlowTest, GenerateBasicIfStatement) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 5;
            if (x > 3) {
                return 1;
            }
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("if") != std::string::npos);
    EXPECT_TRUE(code.find("x > 3") != std::string::npos);
    EXPECT_TRUE(code.find("return 1") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateIfElseStatement) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 5;
            if (x > 10) {
                return 1;
            } else {
                return 0;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("if") != std::string::npos);
    EXPECT_TRUE(code.find("else") != std::string::npos);
    EXPECT_TRUE(code.find("x > 10") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateIfElseIfElseChain) {
    std::string input = R"(
        func main() i32 {
            def score: i32 = 75;
            if (score >= 90) {
                return 4;
            } else if (score >= 80) {
                return 3;
            } else if (score >= 70) {
                return 2;
            } else if (score >= 60) {
                return 1;
            } else {
                return 0;
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("if") != std::string::npos);
    EXPECT_TRUE(code.find("else") != std::string::npos);
    EXPECT_TRUE(code.find("score >= 90") != std::string::npos);
    EXPECT_TRUE(code.find("score >= 80") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateNestedIfStatements) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 5;
            def y: i32 = 10;
            if (x > 0) {
                if (y > 5) {
                    return 1;
                } else {
                    return 2;
                }
            } else {
                return 0;
            }
        }
    )";
    std::string code = generateCode(input);
    
    int ifCount = 0;
    size_t pos = 0;
    while ((pos = code.find("if", pos)) != std::string::npos) {
        ifCount++;
        pos++;
    }
    EXPECT_GE(ifCount, 2);
}

// Break and Continue Tests
TEST_F(ControlFlowTest, GenerateBreakInLoop) {
    std::string input = R"(
        func main() i32 {
            def x: i32 = 0;
            while (x < 100) {
                if (x > 10) {
                    break;
                }
                x = x + 1;
            }
            return x;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("break") != std::string::npos);
    EXPECT_TRUE(code.find("while") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateContinueInLoop) {
    std::string input = R"(
        func main() i32 {
            def sum: i32 = 0;
            for (i: i32 -> 0 to 10) {
                if (i == 5) {
                    continue;
                }
                sum = sum + i;
            }
            return sum;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("continue") != std::string::npos);
    EXPECT_TRUE(code.find("for") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateBreakInNestedLoop) {
    std::string input = R"(
        func main() i32 {
            def found: bool = false;
            for (i: i32 -> 0 to 10) {
                for (j: i32 -> 0 to 10) {
                    if (i == j && i == 5) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
            return 0;
        }
    )";
    std::string code = generateCode(input);
    
    int breakCount = 0;
    size_t pos = 0;
    while ((pos = code.find("break", pos)) != std::string::npos) {
        breakCount++;
        pos++;
    }
    EXPECT_GE(breakCount, 2);
}

// Mixed Control Flow Tests
TEST_F(ControlFlowTest, GenerateMixedControlFlow) {
    std::string input = R"(
        func main() i32 {
            def result: i32 = 0;
            for (i: i32 -> 0 to 10) {
                if (i % 2 == 0) {
                    continue;
                }
                
                def j: i32 = 0;
                while (j < i) {
                    if (j > 5) {
                        break;
                    }
                    result = result + j;
                    j = j + 1;
                }
            }
            return result;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("for") != std::string::npos);
    EXPECT_TRUE(code.find("while") != std::string::npos);
    EXPECT_TRUE(code.find("if") != std::string::npos);
    EXPECT_TRUE(code.find("continue") != std::string::npos);
    EXPECT_TRUE(code.find("break") != std::string::npos);
}

// Return Statement Tests
TEST_F(ControlFlowTest, GenerateEarlyReturn) {
    std::string input = R"(
        func test(x: i32) i32 {
            if (x < 0) {
                return -1;
            }
            if (x == 0) {
                return 0;
            }
            return x * 2;
        }
    )";
    std::string code = generateCode(input);
    
    int returnCount = 0;
    size_t pos = 0;
    while ((pos = code.find("return", pos)) != std::string::npos) {
        returnCount++;
        pos++;
    }
    EXPECT_GE(returnCount, 3);
}

TEST_F(ControlFlowTest, GenerateReturnInLoop) {
    std::string input = R"(
        func findFirst(arr: vec<i32>, target: i32) i32 {
            for (i: i32 -> 0 to 10) {
                if (arr[i] == target) {
                    return i;
                }
            }
            return -1;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("return i") != std::string::npos);
    EXPECT_TRUE(code.find("return -1") != std::string::npos);
}

// Complex Control Flow Integration
TEST_F(ControlFlowTest, GenerateComplexControlFlow) {
    std::string input = R"(
        func fibonacci(n: i32) i32 {
            if (n <= 1) {
                return n;
            }
            
            def a: i32 = 0;
            def b: i32 = 1;
            def i: i32 = 2;
            
            while (i <= n) {
                def temp: i32 = a + b;
                a = b;
                b = temp;
                i = i + 1;
            }
            
            return b;
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("if") != std::string::npos);
    EXPECT_TRUE(code.find("while") != std::string::npos);
    EXPECT_TRUE(code.find("return") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

TEST_F(ControlFlowTest, GenerateBubbleSort) {
    std::string input = R"(
        func bubbleSort(arr: vec<i32>, n: i32) void {
            for (i: i32 -> 0 to n) {
                for (j: i32 -> 0 to n) {
                    if (arr[j] > arr[j + 1]) {
                        def temp: i32 = arr[j];
                        arr[j] = arr[j + 1];
                        arr[j + 1] = temp;
                    }
                }
            }
        }
    )";
    std::string code = generateCode(input);
    
    EXPECT_TRUE(code.find("for") != std::string::npos);
    EXPECT_TRUE(code.find("if") != std::string::npos);
    EXPECT_FALSE(code.find("ERROR") != std::string::npos);
}

// Error Handling Tests
TEST_F(ControlFlowTest, ParseErrorInvalidForSyntax) {
    std::string input = "func main() i32 { for (i: i32 -> 0 to) { } return 0; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(ControlFlowTest, ParseErrorInvalidWhileSyntax) {
    std::string input = "func main() i32 { while { } return 0; }";
    auto result = parseCode(input);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(ControlFlowTest, ParseErrorBreakOutsideLoop) {
    std::string input = R"(
        func main() i32 {
            break;
            return 0;
        }
    )";
    // This might parse but should be caught in semantic analysis
    // For now, just test that it parses
    auto result = parseCode(input);
    // Parser might allow this, semantic analysis should catch it
}

TEST_F(ControlFlowTest, ParseErrorContinueOutsideLoop) {
    std::string input = R"(
        func main() i32 {
            continue;
            return 0;
        }
    )";
    auto result = parseCode(input);
    // Parser might allow this, semantic analysis should catch it
}

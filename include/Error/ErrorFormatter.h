#ifndef ERROR_FORMATTER_H
#define ERROR_FORMATTER_H

#include <string>
#include <sstream>
#include "backend/Tokenizer.h"

namespace ArgonLang {
    class ErrorFormatter {
    public:
        // Format parsing errors consistently
        static std::string formatParseError(
            const std::string& context,
            const std::string& expected,
            const Token& actual
        ) {
            std::ostringstream oss;
            oss << context << ": expected " << expected 
                << ", but got '" << actual.value << "' (" << Token::getTypeAsString(actual.type) << ")";
            return oss.str();
        }

        // Format tokenizer errors consistently
        static std::string formatTokenizerError(
            const std::string& issue,
            size_t line,
            size_t column
        ) {
            std::ostringstream oss;
            oss << "Tokenizer error at line " << line << ", column " << column << ": " << issue;
            return oss.str();
        }

        // Format semantic analysis errors
        static std::string formatSemanticError(
            const std::string& issue,
            const std::string& context,
            const Token::Position& position
        ) {
            std::ostringstream oss;
            oss << "Semantic error at line " << position.line << ", column " << position.column;
            if (!context.empty()) {
                oss << " in " << context;
            }
            oss << ": " << issue;
            return oss.str();
        }

        // Format code generation errors
        static std::string formatCodeGenError(
            const std::string& issue,
            const std::string& nodeType
        ) {
            std::ostringstream oss;
            oss << "Code generation error in " << nodeType << ": " << issue;
            return oss.str();
        }

        // Format position information
        static std::string formatPosition(const Token::Position& pos) {
            std::ostringstream oss;
            oss << "line " << pos.line << ", column " << pos.column;
            return oss.str();
        }

        // Create error notes for suggestions
        static std::string createSuggestion(const std::string& suggestion) {
            return "Suggestion: " + suggestion;
        }

        // Create error notes for context
        static std::string createContext(const std::string& context) {
            return "Context: " + context;
        }
    };
}

#endif // ERROR_FORMATTER_H

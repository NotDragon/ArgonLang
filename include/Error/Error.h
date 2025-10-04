//
// Created by User on 13/12/2024.
//

#ifndef ARGONLANG_ERROR_H
#define ARGONLANG_ERROR_H

#include <string>
#include <vector>
#include <optional>
#include <expected>
#include <variant>
#include <format>
#include <functional>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "backend/Tokenizer.h"

namespace ArgonLang {

// Error severity levels
enum class ErrorSeverity {
    Warning,
    Error,
    Fatal
};

// Specific error types for different phases
enum class ErrorType {
    // Lexical errors
    UnexpectedCharacter,
    UnterminatedString,
    UnterminatedComment,
    InvalidNumberLiteral,
    
    // Parse errors
    UnexpectedToken,
    MissingToken,
    MissingClosingBracket,
    MissingClosingParen,
    MissingClosingBrace,
    InvalidExpression,
    InvalidStatement,
    InvalidType,
    InvalidFunctionSignature,
    InvalidGenericParameters,
    InvalidArrayLiteral,
    InvalidStructLiteral,
    InvalidLambdaExpression,
    
    // Type system errors
    TypeMismatch,
    UndefinedVariable,
    UndefinedFunction,
    UndefinedType,
    UndefinedGenericParameter,
    InvalidGenericConstraint,
    CircularTypeDefinition,
    InvalidTypeConversion,
    InvalidGenericInstantiation,
    ConstraintNotSatisfied,
    
    // Code generation errors
    InvalidCodeGeneration,
    UnsupportedFeature,
    InternalCompilerError,
    
    // Runtime errors (for future use)
    DivisionByZero,
    NullPointerDereference,
    IndexOutOfBounds,
    StackOverflow,
    HeapOverflow
};

// Position information with source context
struct Position {
    std::string filename;
    size_t line;
    size_t column;
    std::string source_line;  // The actual line content
    size_t token_start;       // Start of the problematic token
    size_t token_end;         // End of the problematic token
    
    Position() : filename(""), line(0), column(0), token_start(0), token_end(0) {}
    Position(const std::string& filename, size_t line, size_t column) 
        : filename(filename), line(line), column(column), token_start(column), token_end(column) {}
    Position(const std::string& filename, size_t line, size_t column, 
             const std::string& source_line, size_t token_start, size_t token_end)
        : filename(filename), line(line), column(column), 
          source_line(source_line), token_start(token_start), token_end(token_end) {}
    
    std::string to_string() const {
        return std::format("{}:{}:{}", filename, line, column);
    }
};

// Range information for multi-token errors
struct Range {
    Position start;
    Position end;
    
    Range() = default;
    Range(const Position& start, const Position& end) : start(start), end(end) {}
    
    std::string to_string() const {
        if (start.line == end.line) {
            return std::format("{}:{}:{}-{}", start.filename, start.line, start.column, end.column);
        }
        return std::format("{}:{}:{}-{}:{}:{}", start.filename, start.line, start.column, 
                          end.filename, end.line, end.column);
    }
};

// Visual indicator types for different error types
enum class IndicatorType {
    caret,      // ^ for single tokens
    tilde,      // ~~ for ranges
    arrow,      // --> for multi-line spans
    brace       // {} for matching issues
};

// Source code snippet for error display
struct SourceSnippet {
    std::string filename;
    size_t line_number;
    std::string source_line;           // The actual source code line
    size_t column_start;               // Start of problematic area
    size_t column_end;                 // End of problematic area
    IndicatorType indicator_type;      // Type of visual indicator
    std::string underline;            // Visual indicator (^, ~~, etc.)
    std::vector<std::string> context_before; // Lines before for context
    std::vector<std::string> context_after;  // Lines after for context
    std::string highlight_message;     // Message to show above the indicator
    
    SourceSnippet() : line_number(0), column_start(0), column_end(0), indicator_type(IndicatorType::caret) {}
};

// Source file manager for loading and caching source files
class SourceManager {
    std::unordered_map<std::string, std::vector<std::string>> file_contents;
    std::unordered_map<std::string, bool> file_loaded;
    
public:
    // Load a file into memory
    void load_file(const std::string& filename) {
        if (file_loaded[filename]) return;
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            file_loaded[filename] = false;
            return;
        }
        
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        
        file_contents[filename] = std::move(lines);
        file_loaded[filename] = true;
    }
    
    // Get a specific line from a file
    std::string get_line(const std::string& filename, size_t line_number) {
        load_file(filename);
        if (!file_loaded[filename] || line_number == 0 || line_number > file_contents[filename].size()) {
            return "";
        }
        return file_contents[filename][line_number - 1];
    }
    
    // Get context lines around a specific line
    std::vector<std::string> get_context_lines(const std::string& filename, 
                                           size_t line_number, 
                                           size_t context_lines = 3) {
        load_file(filename);
        if (!file_loaded[filename] || line_number == 0 || line_number > file_contents[filename].size()) {
            return {};
        }
        
        std::vector<std::string> context;
        size_t start = (line_number > context_lines) ? line_number - context_lines : 1;
        size_t end = std::min(line_number + context_lines, file_contents[filename].size());
        
        for (size_t i = start; i <= end; ++i) {
            context.push_back(file_contents[filename][i - 1]);
        }
        
        return context;
    }
    
    // Create a source snippet for error display
    SourceSnippet create_snippet(const std::string& filename, 
                               size_t line, 
                               size_t col_start, 
                               size_t col_end,
                               IndicatorType indicator_type = IndicatorType::caret,
                               const std::string& highlight_message = "") {
        SourceSnippet snippet;
        snippet.filename = filename;
        snippet.line_number = line;
        snippet.source_line = get_line(filename, line);
        snippet.column_start = col_start;
        snippet.column_end = col_end;
        snippet.indicator_type = indicator_type;
        snippet.highlight_message = highlight_message;
        
        // Get context lines
        auto context_lines = get_context_lines(filename, line, 3);
        if (!context_lines.empty()) {
            size_t context_start = (line > 3) ? line - 3 : 1;
            snippet.context_before.clear();
            snippet.context_after.clear();
            
            for (size_t i = 0; i < context_lines.size(); ++i) {
                size_t context_line_num = context_start + i;
                if (context_line_num < line) {
                    snippet.context_before.push_back(context_lines[i]);
                } else if (context_line_num > line) {
                    snippet.context_after.push_back(context_lines[i]);
                }
            }
        }
        
        // Generate visual indicator
        snippet.underline = generate_indicator(col_start, col_end, indicator_type);
        
        return snippet;
    }
    
private:
    std::string generate_indicator(size_t start, size_t end, IndicatorType type) {
        switch (type) {
            case IndicatorType::caret:
                return std::string(end - start, '^');
            case IndicatorType::tilde:
                return std::string(end - start, '~');
            case IndicatorType::arrow:
                return std::string(end - start, '-') + ">";
            case IndicatorType::brace:
                return std::string(end - start, '^');
            default:
                return std::string(end - start, '^');
        }
    }
};

// Error context information
struct ErrorContext {
    std::string expected;
    std::string actual;
    std::optional<std::string> suggestion;
    std::vector<std::string> notes;
    
    ErrorContext() = default;
    ErrorContext(const std::string& expected, const std::string& actual, 
                const std::optional<std::string>& suggestion = std::nullopt)
        : expected(expected), actual(actual), suggestion(suggestion) {}
    
    void add_note(const std::string& note) {
        notes.push_back(note);
    }
};

// Comprehensive error structure
struct Error {
    ErrorType type;
    std::string message;
    Position position;
    std::optional<Range> range;
    ErrorSeverity severity;
    ErrorContext context;
    std::vector<Error> causes; // For error chaining/propagation
    std::optional<SourceSnippet> snippet; // Source code snippet for display
    
    Error() : type(ErrorType::InternalCompilerError), severity(ErrorSeverity::Fatal) {}
    
    Error(ErrorType type, const std::string& message, const Position& position,
          ErrorSeverity severity = ErrorSeverity::Error)
        : type(type), message(message), position(position), severity(severity) {}
    
    Error(ErrorType type, const std::string& message, const Range& range,
          ErrorSeverity severity = ErrorSeverity::Error)
        : type(type), message(message), range(range), severity(severity) {
        position = range.start;
    }
    
    // Add error context
    Error& withExpected(const std::string& expected) {
        context.expected = expected;
        return *this;
    }
    
    Error& withActual(const std::string& actual) {
        context.actual = actual;
        return *this;
    }
    
    Error& withSuggestion(const std::string& suggestion) {
        context.suggestion = suggestion;
        return *this;
    }
    
    Error& with_note(const std::string& note) {
        context.add_note(note);
        return *this;
    }
    
    Error& caused_by(const Error& cause) {
        causes.push_back(cause);
        return *this;
    }
    
    // Add source snippet
    Error& with_snippet(const SourceSnippet& source_snippet) {
        snippet = source_snippet;
        return *this;
    }
    
    Error& with_snippet(SourceSnippet&& source_snippet) {
        snippet = std::move(source_snippet);
        return *this;
    }
    
    // Get formatted error message
    std::string get_formatted_message() const {
        std::string result = position.to_string() + ": " + get_severity_string() + ": " + message;
        
        if (!context.expected.empty() || !context.actual.empty()) {
            result += "\n";
            if (!context.expected.empty()) {
                result += "  Expected: " + context.expected + "\n";
            }
            if (!context.actual.empty()) {
                result += "  Actual: " + context.actual + "\n";
            }
        }
        
        if (context.suggestion.has_value()) {
            result += "  Suggestion: " + context.suggestion.value() + "\n";
        }
        
        for (const auto& note : context.notes) {
            result += "  Note: " + note + "\n";
        }
        
        // Add source snippet if available
        if (snippet.has_value()) {
            result += "\n" + format_source_snippet(snippet.value());
        }
        
        for (const auto& cause : causes) {
            result += "  Caused by: " + cause.get_formatted_message() + "\n";
        }
        
        return result;
    }
    
private:
    std::string get_severity_string() const {
        switch (severity) {
            case ErrorSeverity::Warning: return "warning";
            case ErrorSeverity::Error: return "error";
            case ErrorSeverity::Fatal: return "fatal error";
            default: return "unknown";
        }
    }
    
    std::string format_source_snippet(const SourceSnippet& snippet) const {
        std::string result;
        
        // Add context lines before
        size_t context_line_num = snippet.line_number - snippet.context_before.size();
        for (const auto& line : snippet.context_before) {
            result += std::format("{} | {}\n", context_line_num, line);
            context_line_num++;
        }
        
        // Add the main line with error
        result += std::format("{} | {}\n", snippet.line_number, snippet.source_line);
        
        // Add the underline indicator
        std::string spaces(snippet.column_start - 1, ' ');
        result += std::format("  | {}{}", spaces, snippet.underline);
        
        // Add highlight message if present
        if (!snippet.highlight_message.empty()) {
            result += std::format(" {}", snippet.highlight_message);
        }
        result += "\n";
        
        // Add context lines after
        context_line_num = snippet.line_number + 1;
        for (const auto& line : snippet.context_after) {
            result += std::format("{} | {}\n", context_line_num, line);
            context_line_num++;
        }
        
        return result;
    }
};

// Convenience function to create common errors
inline Error create_parse_error(ErrorType type, const std::string& message, 
                            const Token::Position& token_pos, const std::string& filename = "") {
    Position pos(filename, token_pos.line, token_pos.column);
    return Error(type, message, pos);
}

inline Error create_parse_error(ErrorType type, const std::string& message, 
                            const Token::Position& start_pos, const Token::Position& end_pos, 
                            const std::string& filename = "") {
    Position start(filename, start_pos.line, start_pos.column);
    Position end(filename, end_pos.line, end_pos.column);
    Range range(start, end);
    return Error(type, message, range);
}

inline Error create_unexpected_token_error(const Token& token, const std::string& expected, 
                                       const std::string& filename = "") {
    Position pos(filename, token.position.line, token.position.column);
    return Error(ErrorType::UnexpectedToken, 
                std::format("Unexpected token '{}'", token.value), pos)
           .withExpected(expected)
           .withActual(token.value);
}

inline Error create_missing_token_error(const std::string& expected, const Token::Position& pos, 
                                    const std::string& filename = "") {
    Position position(filename, pos.line, pos.column);
    return Error(ErrorType::MissingToken, 
                std::format("Expected '{}'", expected), position)
           .withExpected(expected);
}

// Enhanced error creation with source snippets
inline Error create_parse_error_with_snippet(ErrorType type, const std::string& message, 
                                           const Token& token, SourceManager& source_manager,
                                           IndicatorType indicator_type = IndicatorType::caret,
                                           const std::string& highlight_message = "") {
    Position pos("", token.position.line, token.position.column);
    Error error(type, message, pos);
    
    // Create source snippet
    auto snippet = source_manager.create_snippet("",
                                               token.position.line,
                                               token.position.column,
                                               token.position.column + token.value.length(),
                                               indicator_type,
                                               highlight_message);
    
    return error.with_snippet(std::move(snippet));
}

inline Error create_unexpected_token_error_with_snippet(const Token& token, const std::string& expected, 
                                                      SourceManager& source_manager) {
    Position pos("", token.position.line, token.position.column);
    Error error = Error(ErrorType::UnexpectedToken, 
                       std::format("Unexpected token '{}'", token.value), pos)
                  .withExpected(expected)
                  .withActual(token.value);
    
    // Create source snippet
    auto snippet = source_manager.create_snippet("",
                                               token.position.line,
                                               token.position.column,
                                               token.position.column + token.value.length(),
                                               IndicatorType::caret,
                                               "Unexpected token");
    
    return error.with_snippet(std::move(snippet));
}

} // namespace ArgonLang

#endif // ARGONLANG_ERROR_H

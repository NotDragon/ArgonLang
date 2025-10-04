//
// Created by User on 13/12/2024.
//

#ifndef ARGONLANG_RESULT_H
#define ARGONLANG_RESULT_H

#include <expected>
#include <memory>
#include <type_traits>
#include <functional>
#include "Error.h"

namespace ArgonLang {

// Type alias for our Result type using std::expected
template<typename T>
using Result = std::expected<T, Error>;

// Convenience functions for creating Results

// Success cases
template<typename T>
Result<T> Ok(T&& value) {
    return Result<T>(std::forward<T>(value));
}

template<typename T>
Result<T> Ok(const T& value) {
    return Result<T>(value);
}

// Error cases
template<typename T>
Result<T> Err(ErrorType type, const std::string& message, const Position& position,
              ErrorSeverity severity = ErrorSeverity::Error) {
    return Result<T>(std::unexpected(Error(type, message, position, severity)));
}

template<typename T>
Result<T> Err(ErrorType type, const std::string& message, const Range& range,
              ErrorSeverity severity = ErrorSeverity::Error) {
    return Result<T>(std::unexpected(Error(type, message, range, severity)));
}

template<typename T>
Result<T> Err(const Error& error) {
    return Result<T>(std::unexpected(error));
}

template<typename T>
Result<T> Err(Error&& error) {
    return Result<T>(std::unexpected(std::move(error)));
}

// Convenience functions for common error patterns

template<typename T>
Result<T> UnexpectedToken(const Token& token, const std::string& expected) {
    return Err<T>(create_unexpected_token_error(token, expected));
}

template<typename T>
Result<T> MissingToken(const std::string& expected, const Token::Position& pos) {
    return Err<T>(create_missing_token_error(expected, pos));
}

template<typename T>
Result<T> ParseError(ErrorType type, const std::string& message, const Token::Position& tokenPos) {
    return Err<T>(create_parse_error(type, message, tokenPos));
}

template<typename T>
Result<T> ParseError(ErrorType type, const std::string& message, 
                    const Token::Position& startPos, const Token::Position& endPos) {
    return Err<T>(create_parse_error(type, message, startPos, endPos));
}

// Error propagation helper
template<typename T>
Result<T> PropagateError(const Error& error) {
    return Err<T>(error);
}

// Monadic operations for Result<T>

// Map operation: apply function to value if successful, otherwise propagate error
template<typename T, typename F>
auto map(Result<T> result, F&& func) -> Result<std::invoke_result_t<F, T>> {
    if (result.has_value()) {
        return Ok(func(result.value()));
    } else {
        return Err<std::invoke_result_t<F, T>>(result.error());
    }
}

// FlatMap operation: apply function that returns Result to value if successful
template<typename T, typename F>
auto flatMap(Result<T> result, F&& func) -> std::invoke_result_t<F, T> {
    if (result.has_value()) {
        return func(result.value());
    } else {
        return Err<std::invoke_result_t<F, T>>(result.error());
    }
}

// AndThen operation (alias for flatMap)
template<typename T, typename F>
auto andThen(Result<T> result, F&& func) -> std::invoke_result_t<F, T> {
    return flatMap(std::move(result), std::forward<F>(func));
}

// OrElse operation: provide alternative if error
template<typename T, typename F>
Result<T> orElse(Result<T> result, F&& func) {
    if (result.has_value()) {
        return result;
    } else {
        return func(result.error());
    }
}

// MapError operation: transform error if present
template<typename T, typename F>
Result<T> mapError(Result<T> result, F&& func) {
    if (result.has_value()) {
        return result;
    } else {
        return Err<T>(func(result.error()));
    }
}

// Helper to unwrap Result with default value
template<typename T>
T unwrapOr(Result<T> result, T&& default_value) {
    if (result.has_value()) {
        return result.value();
    } else {
        return std::forward<T>(default_value);
    }
}

// Helper to unwrap Result or panic (for development)
template<typename T>
T unwrap(Result<T> result) {
    if (result.has_value()) {
        return result.value();
    } else {
        throw std::runtime_error("Result unwrap failed: " + result.error().get_formatted_message());
    }
}

// Helper to unwrap Result or return alternative
template<typename T>
T unwrapOrElse(Result<T> result, std::function<T()> alternative) {
    if (result.has_value()) {
        return result.value();
    } else {
        return alternative();
    }
}

// Type traits for Result
template<typename T>
struct is_result : std::false_type {};

template<typename T>
struct is_result<Result<T>> : std::true_type {};

template<typename T>
constexpr bool is_result_v = is_result<T>::value;

} // namespace ArgonLang

#endif // ARGONLANG_RESULT_H
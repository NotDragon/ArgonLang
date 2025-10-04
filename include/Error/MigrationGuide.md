# Error System Migration Guide

## Overview
We've migrated from a custom `Result<T>` class to using `std::expected<T, Error>` for better error handling with comprehensive error information.

## Key Changes

### 1. Error Structure
Instead of simple string errors, we now have rich error objects:

```cpp
// Old way
Result<ASTNode> parse() {
    return Result<ASTNode>("Parse error", "", trace);
}

// New way
Result<ASTNode> parse() {
    return Err<ASTNode>(ErrorType::ParseError, "Unexpected token", position)
           .withExpected("identifier")
           .withActual("123")
           .withSuggestion("Use a valid identifier name");
}
```

### 2. Result Creation

```cpp
// Success cases
Result<int> success = Ok(42);
Result<std::string> success2 = Ok("hello");

// Error cases
Result<int> error = Err<int>(ErrorType::TypeMismatch, "Expected int, got string", position);
Result<int> error2 = UnexpectedToken<int>(token, "identifier");
Result<int> error3 = MissingToken<int>("]", pos);
```

### 3. Error Handling

```cpp
// Old way
if (result.hasError()) {
    std::cout << result.getErrorMsg() << std::endl;
    return result;
}

// New way
if (!result.has_value()) {
    std::cout << result.error().getFormattedMessage() << std::endl;
    return Err<T>(result.error());
}
```

### 4. Value Access

```cpp
// Old way (unsafe)
auto value = result.getValue(); // Throws if error

// New way (safe)
if (result.has_value()) {
    auto value = result.value();
    // Use value
}

// Or with default
auto value = unwrapOr(result, defaultValue);
```

### 5. Monadic Operations

```cpp
// Map operation
auto result = Ok(42)
    .and_then([](int x) { return Ok(x * 2); })
    .and_then([](int x) { return Ok(std::to_string(x)); });

// Error propagation
auto result = parseExpression()
    .and_then([](auto expr) { return parseStatement(expr); })
    .or_else([](const Error& err) { return Err<Statement>(err); });
```

## Migration Steps

1. Replace `Result<T>` with `Result<T>` (type alias to `std::expected<T, Error>`)
2. Update error creation to use `Err<T>()` functions
3. Replace `hasError()` with `!has_value()`
4. Replace `getValue()` with safe access patterns
5. Use monadic operations for cleaner error handling
6. Update error messages to use the new `Error` structure

## Benefits

- **Type Safety**: Errors are now typed, not just strings
- **Rich Context**: Errors include position, expected/actual values, suggestions
- **Monadic Operations**: Clean error handling with `and_then`, `or_else`, etc.
- **Standard Library**: Uses `std::expected` from C++23
- **Error Chaining**: Errors can have causes and context
- **Severity Levels**: Errors can be warnings, errors, or fatal

# ArgonLang Expression Precedence Order

This document lists all expression operators in ArgonLang ordered from **lowest precedence** (evaluated last) to **highest precedence** (evaluated first).

## Precedence Order (Lowest to Highest)

### 1. Assignment Operators (Lowest Precedence)
- `=`, `+=`, `-=`, `*=`, `/=`, `%=`
- `*&=`, `*|=`, `*^=`, `*<=`, `*>=` (bitwise assignments)
- `|=`, `&=`, `|>=`, `||>=` (functional assignments)
- **Function**: `parse_assignment_expression()`
- **Calls**: `parse_match_expression()`
- **Associativity**: Right-associative

### 2. Pattern Matching
- `=>` (match expression)
- **Function**: `parse_match_expression()`
- **Calls**: `parse_ternary_expression()` for the value being matched
- **Associativity**: Right-associative

### 3. Ternary Operator
- `?? :` (conditional expression - uses `??` not `?`)
- **Function**: `parse_ternary_expression()`
- **Calls**: `parse_logical_or_expression()` for condition
- **Associativity**: Right-associative

### 4. Logical OR
- `||` (logical OR)
- **Function**: `parse_logical_or_expression()`
- **Calls**: `parse_logical_and_expression()`
- **Associativity**: Left-associative

### 5. Logical AND
- `&&` (logical AND)
- **Function**: `parse_logical_and_expression()`
- **Calls**: `parse_equality_expression()`
- **Associativity**: Left-associative

### 6. Equality Operators
- `==`, `!=` (equal, not equal)
- **Function**: `parse_equality_expression()`
- **Calls**: `parse_relational_expression()`
- **Associativity**: Left-associative

### 7. Relational Operators
- `<`, `>`, `<=`, `>=` (less, greater, less equal, greater equal)
- **Function**: `parse_relational_expression()`
- **Calls**: `parse_bitwise_expression()`
- **Associativity**: Left-associative

### 8. Bitwise OR/AND/XOR
- `*|`, `*&`, `*^` (bitwise OR, AND, XOR)
- **Function**: `parse_bitwise_expression()`
- **Calls**: `parse_shift_expression()`
- **Associativity**: Left-associative

### 9. Bitwise Shift Operators
- `*<`, `*>` (left shift, right shift)
- **Function**: `parse_shift_expression()`
- **Calls**: `parse_to_expression()`
- **Associativity**: Left-associative

### 10. Range Operators
- `to`, `to=` (range, inclusive range)
- **Function**: `parse_to_expression()`
- **Calls**: `parse_additive_expression()`
- **Associativity**: Left-associative

### 11. Additive Operators
- `+`, `-` (addition, subtraction)
- **Function**: `parse_additive_expression()`
- **Calls**: `parse_multiplicative_expression()`
- **Associativity**: Left-associative

### 12. Multiplicative Operators
- `*`, `/`, `%` (multiplication, division, modulo)
- **Function**: `parse_multiplicative_expression()`
- **Calls**: `parse_bitwise_not_expression()`
- **Associativity**: Left-associative

### 13. Unary Bitwise NOT
- `*~` (bitwise NOT)
- **Function**: `parse_bitwise_not_expression()`
- **Calls**: `parse_logical_not_expression()`
- **Associativity**: Right-associative (prefix)

### 14. Unary Logical NOT
- `!` (logical NOT)
- **Function**: `parse_logical_not_expression()`
- **Calls**: `parse_await_expression()`
- **Associativity**: Right-associative (prefix)

### 15. Await Operator
- `await` (await expression)
- **Function**: `parse_await_expression()`
- **Calls**: `parse_iterator_expression()`
- **Associativity**: Right-associative (prefix)

### 16. Iterator Operator
- `$` (iterator expression)
- **Function**: `parse_iterator_expression()`
- **Calls**: `parse_unary_plus_expression()`
- **Associativity**: Right-associative (prefix)

### 17. Unary Plus
- `+` (unary plus)
- **Function**: `parse_unary_plus_expression()`
- **Calls**: `parse_unary_minus_expression()`
- **Associativity**: Right-associative (prefix)

### 18. Unary Minus
- `-` (unary minus/negation)
- **Function**: `parse_unary_minus_expression()`
- **Calls**: `parse_increment_expression()`
- **Associativity**: Right-associative (prefix)

### 19. Postfix Increment/Decrement
- `++`, `--` (postfix increment, postfix decrement)
- **Function**: `parse_increment_expression()`
- **Calls**: `parse_ownership_expression()`
- **Associativity**: Left-associative (postfix)

### 20. Ownership Operator
- `~` (ownership/move)
- **Function**: `parse_ownership_expression()`
- **Calls**: `parse_reference_expression()`
- **Associativity**: Right-associative (prefix)

### 21. Reference Operators
- `&`, `&&` (immutable reference, mutable reference)
- **Function**: `parse_reference_expression()`
- **Calls**: `parse_deref_expression()`
- **Associativity**: Right-associative (prefix)

### 22. Dereference Operator
- `*` (dereference/pointer)
- **Function**: `parse_deref_expression()`
- **Calls**: `parse_range_expression()`
- **Associativity**: Right-associative (prefix)

### 23. Array Literal/Range Expression
- `[...]` (array literal)
- **Function**: `parse_range_expression()`
- **Calls**: `parse_function_call_expression()`

### 24. Function Call
- `(...)` (function call with arguments)
- **Function**: `parse_function_call_expression()`
- **Calls**: `parse_struct_expression()`

### 25. Struct Literal
- `struct { ... }` (struct literal)
- **Function**: `parse_struct_expression()`
- **Calls**: `parse_index_expression()`

### 26. Index/Slice Operators
- `[...]`, `[..]`, `[start..end]` (index, slice)
- **Function**: `parse_index_expression()`
- **Calls**: `parse_member_access_expression()`

### 27. Member Access
- `.`, `::` (member access, scope resolution)
- **Function**: `parse_member_access_expression()`
- **Calls**: `parse_primary()`
- **Associativity**: Left-associative

### 28. Pipe Operators
- `|>`, `||>` (pipe, map-pipe)
- **Function**: `parse_pipe_expression()`
- **Calls**: `parse_parallel_expression()`

### 29. Parallel Execution
- `par` (parallel expression)
- **Function**: `parse_parallel_expression()`
- **Calls**: `parse_filter_expression()`

### 30. Filter Operator
- `|` (filter range)
- **Function**: `parse_filter_expression()`
- **Calls**: `parse_map_expression()`

### 31. Map Operator
- `&` (map range)
- **Function**: `parse_map_expression()`
- **Calls**: `parse_reduce_expression()`

### 32. Reduce Operator
- `?` (reduce)
- **Function**: `parse_reduce_expression()`
- **Calls**: `parse_logical_or_expression()`

### 33. Primary Expressions (Highest Precedence)
- Literals: integers, floats, strings, chars, booleans
- Identifiers: variable names
- Parenthesized expressions: `(...)`
- Lambda expressions: `(args) -> expr` or `arg -> expr`
- **Function**: `parse_primary()`

### 4. Pipe Operators
- `|>`, `||>` (pipe, map-pipe)
- **Function**: `parse_pipe_expression()`
- **Calls**: `parse_parallel_expression()`

### 5. Parallel Execution
- `par` (parallel expression)
- **Function**: `parse_parallel_expression()`
- **Calls**: `parse_filter_expression()`

### 6. Filter Operator
- `|` (filter range)
- **Function**: `parse_filter_expression()`
- **Calls**: `parse_map_expression()`

### 7. Map Operator
- `&` (map range)
- **Function**: `parse_map_expression()`
- **Calls**: `parse_reduce_expression()`

### 8. Reduce Operator
- `?` (reduce)
- **Function**: `parse_reduce_expression()`
- **Calls**: `parse_logical_or_expression()`

### 9. Logical OR
- `||` (logical OR)
- **Function**: `parse_logical_or_expression()`
- **Calls**: `parse_logical_and_expression()`
- **Associativity**: Left-associative

### 10. Logical AND
- `&&` (logical AND)
- **Function**: `parse_logical_and_expression()`
- **Calls**: `parse_equality_expression()`
- **Associativity**: Left-associative

### 11. Equality Operators
- `==`, `!=` (equal, not equal)
- **Function**: `parse_equality_expression()`
- **Calls**: `parse_relational_expression()`
- **Associativity**: Left-associative

### 12. Relational Operators
- `<`, `>`, `<=`, `>=` (less, greater, less equal, greater equal)
- **Function**: `parse_relational_expression()`
- **Calls**: `parse_bitwise_expression()`
- **Associativity**: Left-associative

### 13. Bitwise OR/AND/XOR
- `*|`, `*&`, `*^` (bitwise OR, AND, XOR)
- **Function**: `parse_bitwise_expression()`
- **Calls**: `parse_shift_expression()`
- **Associativity**: Left-associative

### 14. Bitwise Shift Operators
- `*<`, `*>` (left shift, right shift)
- **Function**: `parse_shift_expression()`
- **Calls**: `parse_to_expression()`
- **Associativity**: Left-associative

### 15. Range Operators
- `to`, `to=` (range, inclusive range)
- **Function**: `parse_to_expression()`
- **Calls**: `parse_additive_expression()`
- **Associativity**: Left-associative

### 16. Additive Operators
- `+`, `-` (addition, subtraction)
- **Function**: `parse_additive_expression()`
- **Calls**: `parse_multiplicative_expression()`
- **Associativity**: Left-associative

### 17. Multiplicative Operators
- `*`, `/`, `%` (multiplication, division, modulo)
- **Function**: `parse_multiplicative_expression()`
- **Calls**: `parse_bitwise_not_expression()`
- **Associativity**: Left-associative

### 18. Unary Bitwise NOT
- `*~` (bitwise NOT)
- **Function**: `parse_bitwise_not_expression()`
- **Calls**: `parse_logical_not_expression()`
- **Associativity**: Right-associative (prefix)

### 19. Unary Logical NOT
- `!` (logical NOT)
- **Function**: `parse_logical_not_expression()`
- **Calls**: `parse_await_expression()`
- **Associativity**: Right-associative (prefix)

### 20. Await Operator
- `await` (await expression)
- **Function**: `parse_await_expression()`
- **Calls**: `parse_iterator_expression()`
- **Associativity**: Right-associative (prefix)

### 21. Iterator Operator
- `$` (iterator expression)
- **Function**: `parse_iterator_expression()`
- **Calls**: `parse_unary_plus_expression()`
- **Associativity**: Right-associative (prefix)

### 22. Unary Plus
- `+` (unary plus)
- **Function**: `parse_unary_plus_expression()`
- **Calls**: `parse_unary_minus_expression()`
- **Associativity**: Right-associative (prefix)

### 23. Unary Minus
- `-` (unary minus/negation)
- **Function**: `parse_unary_minus_expression()`
- **Calls**: `parse_increment_expression()`
- **Associativity**: Right-associative (prefix)

### 24. Postfix Increment/Decrement
- `++`, `--` (postfix increment, postfix decrement)
- **Function**: `parse_increment_expression()`
- **Calls**: `parse_ownership_expression()`
- **Associativity**: Left-associative (postfix)

### 25. Ownership Operator
- `~` (ownership/move)
- **Function**: `parse_ownership_expression()`
- **Calls**: `parse_reference_expression()`
- **Associativity**: Right-associative (prefix)

### 26. Reference Operators
- `&`, `&&` (immutable reference, mutable reference)
- **Function**: `parse_reference_expression()`
- **Calls**: `parse_deref_expression()`
- **Associativity**: Right-associative (prefix)

### 27. Dereference Operator
- `*` (dereference/pointer)
- **Function**: `parse_deref_expression()`
- **Calls**: `parse_range_expression()`
- **Associativity**: Right-associative (prefix)

### 28. Array Literal/Range Expression
- `[...]` (array literal)
- **Function**: `parse_range_expression()`
- **Calls**: `parse_function_call_expression()`

### 29. Function Call
- `(...)` (function call with arguments)
- **Function**: `parse_function_call_expression()`
- **Calls**: `parse_struct_expression()`

### 30. Struct Literal
- `struct { ... }` (struct literal)
- **Function**: `parse_struct_expression()`
- **Calls**: `parse_index_expression()`

### 31. Index/Slice Operators
- `[...]`, `[..]`, `[start..end]` (index, slice)
- **Function**: `parse_index_expression()`
- **Calls**: `parse_member_access_expression()`

### 32. Member Access
- `.`, `::` (member access, scope resolution)
- **Function**: `parse_member_access_expression()`
- **Calls**: `parse_primary()`
- **Associativity**: Left-associative

### 33. Primary Expressions (Highest Precedence)
- Literals: integers, floats, strings, chars, booleans
- Identifiers: variable names
- Parenthesized expressions: `(...)`
- Lambda expressions: `(args) -> expr` or `arg -> expr`
- **Function**: `parse_primary()`

## Notes

- **Associativity**: Most binary operators are left-associative. Ternary operator (`? :`) is right-associative.
- **Unary operators** are right-associative (prefix) or left-associative (postfix).
- **Parentheses** `()` can be used to override precedence.
- **Function calls** have higher precedence than most operators, so `func() + 1` is parsed as `(func()) + 1`.
- **Member access** `.` and `::` have very high precedence, so `obj.method()` is parsed as `(obj.method)()`.

## Example

```argonlang
// This expression:
a + b * c ?? x : y = z

// Is parsed as:
((a + (b * c)) ?? x : y) = z

// Precedence breakdown:
// 1. b * c          (multiplicative - level 12)
// 2. a + (b * c)    (additive - level 11)
// 3. (a + b * c) ?? x : y  (ternary - level 3)
// 4. ((a + b * c) ?? x : y) = z  (assignment - level 1)

// Another example showing ternary with logical operators:
a || b ?? x : y && z

// Is parsed as:
(a || b) ?? x : (y && z)

// Precedence breakdown:
// 1. a || b         (logical OR - level 4)
// 2. y && z         (logical AND - level 5)
// 3. (a || b) ?? x : (y && z)  (ternary - level 3)

// Example showing match expression precedence:
x ?? y : z => { pattern -> result }

// Is parsed as:
(x ?? y : z) => { pattern -> result }

// Precedence breakdown:
// 1. x ?? y : z     (ternary - level 3)
// 2. (x ?? y : z) => { ... }  (match - level 2)
```

## Important Notes

- **Pattern matching** (`=>`) has precedence **between ternary and assignment**. This means:
  - `x ?? y : z => { ... }` is parsed as `(x ?? y : z) => { ... }` (ternary binds tighter)
  - `x => { ... } = value` doesn't make sense, so match binds tighter than assignment
  
- **Ternary operator** (`?? :`) has precedence **between logical operators and match**. This means:
  - `a || b ?? x : y` is parsed as `(a || b) ?? x : y` (logical OR binds tighter)
  - `a ?? x : y => { ... }` is parsed as `(a ?? x : y) => { ... }` (ternary binds tighter than match)

- **Functional operators** (filter `|`, map `&`, reduce `^`, pipe `|>`) have precedence between logical operators and primary expressions, but are typically used in specific contexts.


# ArgonLang Analysis Visitor Specification
# Value-Set-Based Type System and Static Analysis

## Overview

ArgonLang uses a sophisticated value-set-based type system where types are defined by the set of possible values they can contain, not just their structural representation. The AnalysisVisitor performs flow-sensitive static analysis to track value sets, ensure type safety, and prevent runtime errors.

## Core Concepts

### Value Sets
Every type has an associated set of possible values it can represent:

```
i32                    // Set: {-2147483648, ..., 2147483647}
positive<i32>          // Set: {1, 2, 3, ..., 2147483647}
range<1,10>            // Set: {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
non_zero<i32>          // Set: {-2147483648, ..., -1, 1, ..., 2147483647}
ptr<T>                 // Set: {null, valid_addresses}
not_null_ptr<T>        // Set: {valid_addresses}
array<T, N>            // Set: {arrays with valid indices 0..N-1}
```

### Type Compatibility
- A value of type A can be used where type B is expected if: ValueSet(A) ⊆ ValueSet(B)
- Function parameters define the minimum required value set
- Function arguments must have value sets that are subsets of the parameter requirements

### Flow-Sensitive Analysis
The AnalysisVisitor tracks how value sets change through program flow:

```argon
func example(ptr: ptr<i32>) {
    // ptr has value set: {null, valid_addresses}
    
    if (ptr != null) {
        // In this branch: ptr has value set: {valid_addresses}
        def value = *ptr;  // Safe - dereference operator requires not_null_ptr
    }
    // After if: ptr has value set: {null, valid_addresses} again
}
```

## Analysis Visitor Responsibilities

### 1. Value Set Tracking
- Maintain precise value-set information for each variable in scope
- Track value set changes through assignments, conditionals, and operations
- Handle value set unions at control flow merge points

### 2. Operator Validation
Each operator has specific value set requirements:

```
Operator: *        (dereference)
Required: not_null_ptr<T>
Returns:  T

Operator: /        (division)  
Required: (T, non_zero<T>)
Returns:  T

Operator: []       (array index)
Required: (array<T,N>, range<0,N-1>)
Returns:  T

Operator: &&       (mutable reference)
Required: mutable<T>
Returns:  mutable_ref<T>

Operator: &        (immutable reference)  
Required: T
Returns:  const_ref<T>

Operator: ~        (move)
Required: owned<T>
Returns:  T (consumes original)
```

### 3. Function Call Validation
- Check that argument value sets are compatible with parameter requirements
- Handle generic function instantiation with value set constraints
- Validate return type value sets match function signature

### 4. Control Flow Analysis
- Track value set narrowing in conditional branches
- Handle loops and their effect on value sets
- Manage value set widening at loop headers
- Handle early returns, breaks, and continues

### 5. Pattern Matching Validation
- Ensure match patterns cover all possible values in the matched expression's value set
- Track value set narrowing in match branches
- Validate guard conditions maintain type safety

### 6. Memory Safety
- Track object lifetimes and ownership
- Prevent use-after-free through lifetime analysis
- Validate reference safety (no dangling references)
- Ensure move semantics preserve uniqueness

## Value Set Refinement Rules

### Conditional Narrowing
```argon
def x: i32 = getValue();          // x: i32
if (x > 0) {
    // x: positive<i32>
} else if (x < 0) {
    // x: negative<i32>  
} else {
    // x: zero<i32>
}
```

### Assignment Widening
```argon
def x: positive<i32> = 5;         // x: positive<i32>
def y: i32 = x;                   // y: i32 (widened, but still positive value)
```

### Operation Result Types
```argon
def a: positive<i32> = 5;
def b: positive<i32> = 3;
def sum = a + b;                  // sum: positive<i32> (positive + positive = positive)
def diff = a - b;                 // diff: i32 (positive - positive could be negative)
```

## Error Detection

### Compile-Time Errors
1. **Null Dereference**: Attempting `*ptr` when ptr's value set includes null
2. **Division by Zero**: Using `/` or `%` when divisor's value set includes zero  
3. **Array Bounds**: Indexing with a value set that exceeds array bounds
4. **Use After Move**: Accessing a value after it has been moved
5. **Dangling Reference**: Creating references that outlive their referents
6. **Type Mismatch**: Passing incompatible value sets to functions
7. **Incomplete Pattern Match**: Match expressions that don't cover all possible values

### Warning Generation
1. **Unreachable Code**: Branches that can never execute due to value set constraints
2. **Redundant Checks**: Conditions that are always true/false based on value sets
3. **Potential Overflow**: Operations that might exceed type bounds

## Integration with Code Generation

### Type Erasure
Since safety is verified by the AnalysisVisitor, generated C++ can use type-erased representations for performance:

```argon
func process(x: positive<i32>) i32 {
    return x * 2;
}
```

Generates:
```cpp
// Safe to generate type-erased code - analysis verified safety
int32_t process(int32_t x) {
    return x * 2;
}
```

### Runtime Safety for Interop
When interfacing with external C++ code, generate type-safe wrappers:

```cpp
// For external C++ consumption
ArgonLang::Runtime::positive_int process_safe(ArgonLang::Runtime::positive_int x) {
    return ArgonLang::Runtime::positive_int(process(x.get()));
}
```

### Debug Mode
In debug builds, optionally generate runtime assertions:

```cpp
int32_t process(int32_t x) {
    ARGON_ASSERT(x > 0, "Expected positive integer");
    return x * 2;
}
```

## Built-in Value Set Types

### Numeric Constraints
```
positive<T>      // T > 0
negative<T>      // T < 0  
non_zero<T>      // T != 0
range<Min,Max>   // Min <= T <= Max
even<T>          // T % 2 == 0
odd<T>           // T % 2 != 0
```

### Pointer Constraints
```
ptr<T>           // nullable pointer
not_null_ptr<T>  // guaranteed non-null
unique_ptr<T>    // unique ownership
shared_ptr<T>    // shared ownership
```

### Reference Constraints
```
const_ref<T>     // immutable reference (&)
mutable_ref<T>   // mutable reference (&&)
```

### Collection Constraints
```
array<T,N>       // fixed-size array
non_empty<T>     // collections with size > 0
bounded<T,N>     // collections with size <= N
```

### Custom Constraints
Users can define custom value set constraints:

```argon
constraint ValidEmail<T: str> = regExMatch(T, "/^[^@]+@[^@]+\.[^@]+$/");
constraint HttpPort<T: Number> = T >= 1 && T <= 65535;
constraint NonEmptyString<T: str> = T.length > 0;
constraint ValidTriangle<A: Number, B: Number, C: Number> = 
    A + B > C && B + C > A && A + C > B;
constraint SumEquals<A: Number, B: Number, target: Number> = A + B == target;
```

### Constraint Composition
Constraints can be composed to create complex requirements:

```argon
// Mathematical relationships
constraint Positive<T: Number> = T > 0;
constraint Even<T: Integer> = T % 2 == 0;
constraint NonZero<T: Number> = T != 0;

// Composite constraints
type PositiveEven<T> = T & Positive<T> & Even<T>;
type SafeDivisor<T> = T & NonZero<T>;

// Usage
def point: ValidPoint = struct { x = 30, y = 40 }; // OK: 30 + 40 < 100
def invalid: ValidPoint = struct { x = 60, y = 50 }; // Error: 60 + 50 >= 100
```

## Implementation Phases

### Phase 1: Basic Value Sets
- Implement nullable vs non-null pointers
- Add positive/negative/non-zero numeric constraints
- Basic flow-sensitive analysis for conditionals

### Phase 2: Advanced Constraints  
- Array bounds checking
- Range constraints
- Custom constraint definitions

### Phase 3: Ownership Analysis
- Move semantics validation
- Lifetime tracking
- Reference safety

### Phase 4: Advanced Features
- Generic constraint solving
- Complex control flow (loops, exceptions)
- Inter-procedural analysis

## Error Reporting

### Error Message Format
```
Error: Potential null pointer dereference
  --> example.arg:15:20
   |
15 |     def value = *ptr;
   |                 ^^^^
   |
   = note: 'ptr' has type 'ptr<i32>' which includes null
   = help: Check for null before dereferencing: if (ptr != null) { ... }
```

### Suggestion System
- Propose fixes for common value set violations
- Suggest narrowing operations (null checks, range validation)
- Recommend safer alternative operations

## Testing Strategy

### Unit Tests
- Test value set operations (union, intersection, subset)
- Validate flow-sensitive analysis correctness
- Check error detection accuracy

### Integration Tests  
- Full program analysis with complex control flow
- Interop with generated C++ code
- Performance benchmarks for large codebases

### Fuzzing
- Generate random programs and verify analysis consistency
- Test edge cases in value set computations
- Validate error recovery and reporting

---

This specification defines the foundation for ArgonLang's advanced type safety system. The AnalysisVisitor implements these concepts to provide compile-time guarantees about program safety while maintaining runtime performance through type erasure.

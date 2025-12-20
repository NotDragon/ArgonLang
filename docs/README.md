# ArgonLang Programming Language

**ArgonLang** is a revolutionary systems programming language that merges functional programming elegance with systems-level control. It features lazy evaluation, advanced pattern matching, ownership semantics, and compile-time safety guarantees while maintaining C++ performance.

## Table of Contents

- [Core Concepts](#core-concepts)
- [Variable System](#variable-system)  
- [Function Paradigms](#function-paradigms)
- [Lazy Collections & Ranges](#lazy-collections--ranges)
- [Functional Operations](#functional-operations)
- [Pattern Matching Engine](#pattern-matching-engine)
- [Object System](#object-system)
- [Memory Ownership](#memory-ownership)
- [Concurrent Programming](#concurrent-programming)
- [Type Safety & Constraints](#type-safety--constraints)
- [Error Management](#error-management)
- [Module Architecture](#module-architecture)
- [Metaprogramming](#metaprogramming)
- [Development Tools](#development-tools)

## Core Concepts

ArgonLang is built on several revolutionary concepts that distinguish it from traditional systems languages:

### Value-Centric Type System  
Types are defined by the set of possible values they can contain, enabling precise compile-time safety without runtime overhead.

### Ownership Without Complexity
Memory management uses intuitive ownership semantics with reference-extended lifetimes—values stay allocated as long as any reference to them exists, all tracked at compile-time.

### Pattern-First Programming
Pattern matching isn't just a feature—it's the primary way to work with data and control flow.

## Variable System

### Basic Declarations

ArgonLang distinguishes between mutable variables and immutable constants:

```argonlang
// Mutable variable - can be reassigned
def temperature: i32 = 20;
def humidity = 65.5;  // Type inferred as f64

// Immutable constant - cannot be reassigned  
const BOILING_POINT: f64 = 100.0;
const FREEZING_POINT = 0.0;  // Type inferred as f64
```

### Collection Types

ArgonLang provides several collection types, each optimized for different use cases:

```argonlang
// Fixed arrays - stack allocated, compile-time size
def scores: i32[5] = [95, 87, 92, 78, 88];

// Dynamic vectors - heap allocated, runtime resizing
def students: vec<str> = vec<str>();
students.push("Alice");
students.push("Bob");

// Linked lists - efficient insertion/deletion
def tasks: list<Task> = list<Task>();

// Lazy ranges - computed on demand
def fibonacci_range: range<i32> = fibonacci_sequence();
```

### Type Inference Power

ArgonLang's type inference is sophisticated enough to handle complex scenarios:

```argonlang
def numbers = [1, 2, 3, 4, 5];           // Inferred: i32[5]
def mixed = vec<f64>();                  // Explicit generic type
def lazy_evens = 0 to 100 | x -> x % 2 == 0;  // Inferred: range<i32>
```

## Function Paradigms

ArgonLang treats functions as first-class citizens with multiple declaration styles optimized for different use cases.

### Function Declaration Styles

```argonlang
// Traditional function with explicit return type
func calculateArea(width: f64, height: f64) f64 {
    return width * height;
}

// Expression-based function (single expression)
func square(x: f64) f64 -> x * x;

// Pattern matching function
func fibonacci(n: i32) => i32 {
    0 -> 0,
    1 -> 1, 
    _ -> fibonacci(n-1) + fibonacci(n-2)
}

// Function declaration (no implementation)
func processData(data: vec<i32>) vec<i32>;
```

### Higher-Order Functions and Closures

Functions can capture their environment and be passed as arguments:

```argonlang
// Closure that captures local variables
func makeMultiplier(factor: i32) func(i32) i32 {
    return (value: i32) -> value * factor;
}

def double = makeMultiplier(2);
def result = double(5);  // result = 10

// Function accepting other functions
func applyTwice(fn: func(i32) i32, value: i32) i32 {
    return fn(fn(value));
}

def incremented = applyTwice(x -> x + 1, 5);  // result = 7
```

### Reference Parameters and Ownership

ArgonLang provides explicit control over how data is passed to functions:

```argonlang
// Pass by value (creates a copy)
func processValue(data: i32) i32 -> data * 2;

// Pass by immutable reference (read-only access)
func analyzeData(data: &vec<i32>) i32 -> data.length;

// Pass by mutable reference (can modify original)
func sortInPlace(data: &&vec<i32>) {
    // Sorts the original vector
    data.sort();
}

// Take ownership (moves the value)
func consumeData(data: ~vec<i32>) i32 {
    return data.length;  // data is consumed here
}

// Example usage
def numbers = [3, 1, 4, 1, 5];
def length = analyzeData(&numbers);     // numbers still accessible
sortInPlace(&&numbers);                 // numbers is modified
def final_length = consumeData(~numbers); // numbers is moved/consumed
```

## Lazy Collections & Ranges

One of ArgonLang's most powerful features is its lazy-first approach to collections and sequences.

### Range Creation and Operations

Ranges are lazy sequences that compute values on-demand:

```argonlang
// Basic ranges (exclusive end)
def counting = 1 to 10;        // [1, 2, 3, 4, 5, 6, 7, 8, 9]

// Inclusive ranges  
def inclusive = 1 to= 10;      // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

// Infinite ranges (lazy computation)
def naturals = 1 to range<i32>::infinity;
```

### Lazy Transformations

Transformations on ranges are lazy - they don't execute until values are actually needed:

```argonlang
// Filter operation (lazy)
def evens = 1 to 100 | x -> x % 2 == 0;

// Map operation (lazy) 
def doubled = 1 to 50 & x -> x * 2;

// Chained operations (all lazy)
def processed = 1 to 1000
    | x -> x % 3 == 0          // Filter multiples of 3
    & x -> x * x               // Square them
    | x -> x > 100;            // Filter large squares

// Only computed when accessed
print(processed[5]);           // Computes only what's needed
```

### Materialization and Eager Evaluation

Convert lazy ranges to concrete collections when needed:

```argonlang
// Materialize to vector
def concrete_numbers: vec<i32> = 1 to 100;

// Take only first N elements
def first_ten = fibonacci_sequence().take(10).as<vec<i32>>();

// Skip and take for windowing
def middle_section = large_dataset.skip(1000).take(100);
```

### Advanced Range Operations

```argonlang
// Zip ranges together
def coordinates = (1 to 10).zip(10 to 1);  // [(1,10), (2,9), (3,8), ...]

// Enumerate with indices
def indexed = ["a", "b", "c"].enumerate();  // [(0,"a"), (1,"b"), (2,"c")]

// Flatten nested structures
def flattened = [[1,2], [3,4], [5,6]].flatten();  // [1, 2, 3, 4, 5, 6]
```

## Functional Operations

ArgonLang provides a rich set of functional operators that work seamlessly with its lazy evaluation system.

### Core Functional Operators

```argonlang
def numbers = 1 to= 20;

// Filter operator |: Keep elements that match condition
def evens = numbers | x -> x % 2 == 0;

// Map operator &: Transform each element
def squares = numbers & x -> x * x;

// Reduce operator ?: Combine elements into single value
def sum = numbers ? (acc, current) -> acc + current;
```

### Pipeline Operations

The pipe operator creates elegant data transformation chains:

```argonlang
// Traditional approach
def temp1 = filter_primes(numbers);
def temp2 = square_values(temp1);  
def result = sum_all(temp2);

// Pipeline approach
def result = numbers
    |> filter_primes
    |> square_values
    |> sum_all;

// Complex pipeline with lambdas
def analysis = sensor_data
    |> clean_outliers
    |> normalize_values  
    & x -> x * calibration_factor
    |> calculate_statistics;
```

### Map-Pipe and Aggregate Operations

```argonlang
// Map-pipe operator ||>: Map then pipe
def account_balances = [
    [100, 200, 50],
    [300, 150],  
    [75, 25, 125, 200]
];

def total_wealth = account_balances ||> sum |> max;
// Equivalent to: account_balances & sum |> max

// Aggregate operator />: Create tuples from multiple operations  
def statistics = dataset /> mean /> median /> std_dev;
// Creates tuple: (mean_val, median_val, std_dev_val)
```

### Advanced Functional Patterns

```argonlang
// Partial application with underscore
func add(a: i32, b: i32) -> a + b;
def add_five = add(5, _);
def result = add_five(10);  // result = 15

// Function composition
func compose<A, B, C>(f: func(B) C, g: func(A) B) func(A) C {
    return (x: A) -> f(g(x));
}

def double = (x: i32) -> x * 2;
def increment = (x: i32) -> x + 1;
def double_then_increment = compose(increment, double);

// Currying for reusable functions
func filter_by<T>(predicate: func(T) bool) func(vec<T>) vec<T> {
    return (data: vec<T>) -> data | predicate;
}

def filter_positives = filter_by((x: i32) -> x > 0);
def positive_numbers = filter_positives([-2, -1, 0, 1, 2, 3]);
```

## Pattern Matching Engine

ArgonLang's pattern matching system is one of its most distinctive features, providing a powerful way to destructure and analyze data.

### Basic Pattern Matching

Pattern matching uses the `=>` operator to match values against patterns:

```argonlang
// Simple value matching
def classify_number = (n: i32) => str {
    0 -> "zero",
    1 -> "one", 
    2 -> "two",
    _ -> "many"
};

// Type-based matching
def describe_value = (value: any) => str {
    i32 -> "integer",
    f64 -> "floating point",
    str -> "text",
    bool -> "boolean",
    _ -> "unknown type"
};
```

### Destructuring Patterns

Extract values from complex data structures:

```argonlang
// Array destructuring
def analyze_coordinates = (point: [f64; 3]) => str {
    [0.0, 0.0, 0.0] -> "origin",
    [x, 0.0, 0.0] -> "on x-axis: " + x,
    [0.0, y, 0.0] -> "on y-axis: " + y, 
    [x, y, z] -> "3D point: (" + x + ", " + y + ", " + z + ")"
};

// Advanced array patterns
def process_list = (items: vec<i32>) => str {
    [] -> "empty list",
    [single] -> "one item: " + single,
    [first, second], rest -> "starts with: " + first + ", " + second,
    [head], tail -> "head: " + head + ", tail length: " + tail.length
};
```

### Struct Pattern Matching

```argonlang
// Struct destructuring
def point = struct { x = 10, y = 20, z = 5 };

def analyze_point = (p: Point3D) => str {
    { x = 0, y = 0, z = 0 } -> "origin",
    { x = 0, y, z } -> "on yz-plane", 
    { x, y = 0, z } -> "on xz-plane",
    { x, y, z = 0 } -> "on xy-plane",
    { x, y, z } -> "general point: (" + x + ", " + y + ", " + z + ")"
};
```

### Guards and Conditional Patterns

Add conditions to patterns for more precise matching:

```argonlang
// Guards with &&
def categorize_triangle = (sides: [f64; 3]) => str {
    [a, b, c] && a == b && b == c -> "equilateral",
    [a, b, c] && (a == b || b == c || a == c) -> "isosceles", 
    [a, b, c] && a*a + b*b == c*c -> "right triangle",
    [a, b, c] && a + b > c && b + c > a && a + c > b -> "valid triangle",
    _ -> "invalid triangle"
};

// Range patterns with guards
def assess_score = (score: i32) => str {
    s && s >= 90 -> "excellent",
    s && s >= 80 -> "good",
    s && s >= 70 -> "satisfactory", 
    s && s >= 60 -> "passing",
    _ -> "failing"
};
```

### Class and Enum Pattern Matching

```argonlang
enum Shape {
    Circle{ radius: f64 },
    Rectangle{ width: f64, height: f64 },
    Triangle{ a: f64, b: f64, c: f64 }
}

func calculate_area(shape: Shape) f64 {
    return shape => {
        Shape::Circle{ radius } -> 3.14159 * radius * radius,
        Shape::Rectangle{ width, height } -> width * height,
        Shape::Triangle{ a, b, c } -> {
            def s = (a + b + c) / 2.0;
            return sqrt(s * (s - a) * (s - b) * (s - c));
        }
    };
}

// Pattern matching with additional conditions
func describe_shape(shape: Shape) str {
    return shape => {
        Shape::Circle{ radius } && radius > 10.0 -> "large circle",
        Shape::Circle{ radius } -> "small circle", 
        Shape::Rectangle{ width, height } && width == height -> "square",
        Shape::Rectangle{ width, height } -> "rectangle",
        Shape::Triangle{ a, b, c } && a == b && b == c -> "equilateral triangle",
        _ -> "some triangle"
    };
}
```

## Functional Programming

### Lambda Expressions

```argonlang
// Anonymous functions
def square = (x: i32) -> x * x;
def add = (a: i32, b: i32) -> a + b;

// Short form for single parameter
def double = x -> x * 2;

// Function types
def operation: func(i32, i32) i32 = add;
```

### Functional Operators

```argonlang
def numbers = 1 to= 10;

// Filter operator |
def evens = numbers | x -> x % 2 == 0;

// Map operator &
def doubled = numbers & x -> x * 2;

// Reduce operator ?
def sum = numbers ? (acc, current) -> acc + current;

// Pipe operator |>
def result = numbers
    |> filter(x -> x > 5)
    |> map(x -> x * 2)
    |> sum();

// Map-pipe operator ||>
def processed = matrix ||> sum |> max;  // Map sum over matrix, then find max
```

### Partial Application

```argonlang
func add(a: i32, b: i32) -> a + b;
def add10 = add(10, _);  // Partial application
def result = add10(5);   // result = 15
```

## Pattern Matching

### Basic Pattern Matching

```argonlang
// Value matching
def day = (dayNumber) => {
    1 -> "Monday",
    2 -> "Tuesday",
    3 -> "Wednesday",
    _ -> "Unknown Day"
};

// Type matching
def describe = (value) => str {
    i32 -> "Integer",
    str -> "String",
    bool -> "Boolean",
    _ -> "Unknown"
};
```

### Advanced Pattern Matching

```argonlang
// Array patterns
def result = arr => {
    [1, 2], rest -> "Starts with 1, 2",
    [first], rest -> "Single element: " + first,
    [] -> "Empty array",
    _ -> "Other pattern"
};

// Struct patterns
def point = struct { x = 10, y = 20 };
def description = point => {
    { x = 0, y } -> "Y-axis point: " + y,
    { x, y = 0 } -> "X-axis point: " + x,
    { x, y } -> "Point(" + x + ", " + y + ")"
};

// Guards (conditional patterns)
def classify = arr => {
    [_, x, _] && x > 5 -> "Middle element > 5",
    [a, b] && a + b > 10 -> "Sum > 10",
    _ -> "Other"
};

// Range patterns
def categorize = (num) => {
    0 -> "Zero",
    1 to= 10 -> "Small number",
    11 to= 100 -> "Medium number",
    _ -> "Large number"
};
```

## Classes and Objects

### Basic Classes

```argonlang
class Student {
    name: str;
    grade: i32;
    
    // Constructor
    pub constructor(name: str, grade: i32) {
        this.name = name;
        this.grade = grade;
    }
    
    // Methods
    pub func getGrade() i32 {
        return grade;
    }
    
    pub func setGrade(newGrade: i32) {
        grade = newGrade;
    }
}

// Creating instances
def student = Student("Alice", 95);
print(student.getGrade());
```

### Advanced Class Features

```argonlang
// Constructor with automatic field mapping
class Point(x: i32, y: i32) {
    const x: i32;  // Automatically mapped from constructor
    const y: i32;  // Automatically mapped from constructor
    
    pub func distance() f64 {
        return Math::sqrt(x * x + y * y);
    }
}

// Multiple constructors
class Rectangle {
    width: i32;
    height: i32;
    
    // Default constructor
    pub constructor(width: i32, height: i32) { }
    
    // Named constructor
    pub construct square(side: i32) {
        width = side;
        height = side;
    }
}

// Public classes (all members public by default)
class Data pub {
    value: i32 = 0;
    func increment() -> value++;
}
```Ok

### Inheritance and Interfaces

```argonlang
// Interface definition
inter Drawable {
    func draw() void;
}

// Class implementing interface
class Circle(radius: i32) impl Drawable {
    const radius: i32;
    
    func draw() void {
        print("Drawing circle with radius " + radius);
    }
}

// Multiple interfaces
class Shape {
    color: str;
}

class ColoredCircle impl Drawable, Shape {
    radius: i32;
    
    func draw() void {
        print("Drawing " + color + " circle");
    }
}
```

### Inline Objects

```argonlang
// Struct literals
def person = struct { 
    name = "John", 
    age = 30 
};

// Struct with interface
def drawable = struct Drawable {
    radius: i32 = 10,
    func draw() void {
        print("Drawing circle");
    }
};
```

## Memory Management

### Automatic Lifetime Management

ArgonLang uses a sophisticated lifetime system that provides memory safety without manual annotations:

**Core Principle**: Variables become unavailable at scope end, but their memory is only deallocated when all references to them go out of scope.

```argonlang
func example() {
    def data = "important";
    def reference: &str;
    
    {
        def inner = "temporary";
        reference = &inner;
    }  // 'inner' unavailable after this scope
       // BUT memory NOT deallocated - 'reference' still holds it
    
    print(reference);  // SAFE - 'inner' memory still alive
    
}  // 'reference' dies, NOW 'inner' memory is deallocated
```

**Key Rules:**
1. Variables become **unavailable** (can't access name) at scope end
2. Memory is **deallocated** only when all references to it die
3. Compiler tracks this at **compile-time** - no runtime overhead
4. Prevents dangling references without lifetime annotations
5. This applies to **all non-pointer variables** (stack-allocated values)

**Examples:**

```argonlang
// Single reference extends lifetime
func get_reference() {
    def outer_ref: &i32;
    {
        def value = 42;
        outer_ref = &value;
        // 'value' name becomes unavailable at scope end
    }  // 'value' unavailable, but memory kept alive for outer_ref
    
    print(outer_ref);  // SAFE - value's memory still exists
    
}  // outer_ref dies, NOW value's memory is deallocated

// Multiple references
func multi_ref() {
    def data = vec<i32>();
    def ref1 = &data;
    def ref2 = &data;
    // data's memory stays allocated until BOTH ref1 and ref2 die
}

// Returning references
func get_first(x: &str, y: &str) &str {
    return x.length > y.length ? x : y;
}

func safe_usage() {
    def s1 = "long string";
    def result: &str;
    {
        def s2 = "short";
        result = get_first(&s1, &s2);
        // s2 becomes unavailable (can't use name 's2')
    }  // BUT s2's memory NOT deallocated - 'result' still references it
    
    print(result);  // SAFE - s2's memory still alive
    
}  // 'result' dies, NOW s2's memory is deallocated
```

### Pointer Types

```argonlang
// Raw pointers (C-style) - manual management
def ptr: *i32;           // Raw pointer
def arr: *i32[5];        // Pointer to array

// Owned pointers (RAII - automatic cleanup)
def owned: ~i32 = 42;    // Owned pointer

// Reference counting (runtime)
def shared: ref<i32> = owned;    // Shared reference
def weak: weak<i32> = shared;    // Weak reference
```

### Reference Semantics

```argonlang
func processValue(value: i32) { }        // Pass by value
func processRef(value: &i32) { }         // Immutable reference
func processMut(value: &&i32) { }        // Mutable reference
func takeOwnership(value: ~i32) { }      // Move semantics

def x = 42;
processValue(x);        // Copy
processRef(&x);         // Borrow immutably
processMut(&&x);        // Borrow mutably
takeOwnership(~x);      // Move ownership
```

### Memory Safety and Borrow Rules

```argonlang
// Ownership rules enforced at compile time
func example() {
    def data: ~i32 = 42;
    def ref1 = &data;     // Immutable reference
    def ref2 = &data;     // Multiple immutable refs OK
    
    // def mutRef = &&data;  // Error: cannot have mutable ref with immutable refs
}

// Reference-extended lifetimes prevent use-after-free
func safe_references() {
    def outer: &vec<i32>;
    {
        def inner = vec<i32>();
        inner.push(1);
        inner.push(2);
        outer = &inner;
        // 'inner' name unavailable after this
    }  // inner's MEMORY kept alive for 'outer'
    
    print(outer[0]);  // SAFE - memory still exists
    
}  // 'outer' dies, inner's memory NOW deallocated

// Mutability tracking
class Container {
    mut data: vec<i32>;  // Mutable field
    
    func add(value: i32) mut {  // Method that mutates
        data.push(value);
    }
    
    func get(index: i32) i32 {  // Non-mutating method
        return data[index];
    }
}
```

### Custom Allocators

```argonlang
// Heap allocation (default)
def ptr: *i32 = alloc::heap::alloc(42);

// Stack buffer allocation
def buffer: bytes[1000];
def stackPtr: *i32 = alloc::buffer<buffer>::alloc(20);

// Arena allocation
def arena = alloc::bump();
def arenaPtr: *i32 = arena::alloc(42);

// Function with custom allocator
def funcArena = alloc::bump();
func processData() use funcArena {
    // All allocations use funcArena
}
```

## Generics and Constraints

### Generic Functions

```argonlang
// Basic generic function
func identity<T>(value: T) T {
    return value;
}

// Constrained generics
func add<T: Number>(a: T, b: T) T {
    return a + b;
}

// Multiple constraints
func process<T: Number & Positive>(value: T) T {
    return value * 2;
}

// Concrete type constraints
func processInt<T: i32>(value: T) T {
    return value + 1;
}
```

### Generic Classes

```argonlang
// Generic class
class Container<T> {
    data: T;
    
    pub constructor(data: T) { }
    
    pub func get() T {
        return data;
    }
}

// Constrained generic class
class NumericContainer<T: Number> {
    value: T;
    
    pub func add(other: T) T {
        return value + other;
    }
}

// Usage
def intContainer = Container<i32>(42);
def floatContainer = Container<f64>(3.14);
```

### Custom Constraints

```argonlang
// Define constraints
constraint Positive<T: Number> = T > 0;
constraint Even<T: Number> = T % 2 == 0;
constraint NonZero<T: Number> = T != 0;

// Complex constraints
constraint ValidTriangle<A: Number, B: Number, C: Number> = 
    A + B > C && B + C > A && A + C > B;

// Usage in functions
func safeDivide<T: Number & NonZero>(dividend: T, divisor: T) T {
    return dividend / divisor;  // Safe: divisor cannot be zero
}

func processPositive<T: Number & Positive>(value: T) T {
    return value * 2;  // Safe: value is guaranteed positive
}
```

### Built-in Concepts

```argonlang
// Number concept (arithmetic types)
func calculate<T: Number>(a: T, b: T) T -> a + b;

// Type concept (any type)
func store<T: Type>(value: T) T -> value;

// Custom concepts
constraint Comparable<T> = requires(T a, T b) {
    a < b;
    a > b;
    a == b;
};
```

## Concurrency and Parallelism

### Parallel Execution

```argonlang
// Parallel expression
def future = par expensiveComputation();

// Awaiting results
def result = await future;

// Scoped parallelism
func processData() {
    {
        par task1();
        par task2();
        par task3();
    }  // Scope waits for all parallel tasks to complete
}
```

### Generators and Yield

```argonlang
// Generator function
func fibonacci() vec<i32> {
    def a = 0, b = 1;
    while (true) {
        yield a;
        def temp = a + b;
        a = b;
        b = temp;
    }
}

// Using generators
for (n -> fibonacci().take(10)) {
    print(n);  // First 10 Fibonacci numbers
}
```

### Thread Management

```argonlang
// Manual thread creation
func workerTask(thread: Thread) {
    // Thread work
}

def thread = Thread(workerTask, struct { 
    priority: HIGH, 
    timeout: 1000 
});

// Synchronization primitives
def mutex: std::mutex;
def num: i32 = 0;

func increment() {
    for (i -> 0 to 1000) lock(mutex) {
        num++;  // Automatically locked/unlocked
    }
}
```

### Async Patterns

```argonlang
// Future with callbacks
def data = par fetchData();

onYield(data) {
    // Called on each yield
}

onReturn(data) {
    // Called when complete
}

onError(data) {
    // Called on error
}

// Combining futures
whenAll([future1, future2]) {
    // All completed
}

whenAny([future1, future2]) {
    // Any completed
}
```

## Error Handling

### Try-Catch Pattern

```argonlang
// Function that can throw
func divide(a: i32, b: i32) i32 {
    if (b == 0) {
        throw Error::DivisionByZero;
    }
    return a / b;
}

// Handling errors
def result = divide(10, 2) catch (e) -> 0;  // Default value on error

def result = divide(10, 2) catch (e) -> {
    print("Error: " + e.message);
    return -1;
};
```

### Maybe and Either Types

```argonlang
// Maybe type for nullable values
func findElement(arr: vec<i32>, target: i32) maybe<i32> {
    for (i, value -> arr.enumerate()) {
        if (value == target) {
            return i;
        }
    }
    return maybe::none;
}

// Either type for error handling
func safeDivide(a: i32, b: i32) either<i32, str> {
    if (b == 0) {
        return "Division by zero";
    }
    return a / b;
}

// Pattern matching on results
def result = safeDivide(10, 2) => {
    i32(value) -> "Result: " + value,
    str(error) -> "Error: " + error
};
```

### Contracts and Preconditions

```argonlang
// Function contracts
func divide(a: i32, b: i32) [expects: b != 0, ensures: return == a / b] i32 {
    return a / b;
}
// def result = divide(10, getUserInput()); // Error: Possible contract vialotion
// Usage with contract checking
```

## Modules and Imports

### Module Definition

```argonlang
// math.arg
module math;

const PI = 3.14159;

func add(a: i32, b: i32) -> a + b;
func multiply(a: i32, b: i32) -> a * b;
func factorial(n: i32) -> n <= 1 ? 1 : n * factorial(n - 1);

// Restricted module export
module math_basic = struct { PI, add };
```

### Importing

```argonlang
// Import entire module
import math;

func main() {
    print(math::PI);
    print(math::add(5, 3));
}

// Import specific items
import math -> PI, add;

func main() {
    print(PI);
    print(add(5, 3));
}

// Using directive
using math;  // Brings all math symbols into scope

func main() {
    print(PI);
    print(add(5, 3));
}

// Standard library imports
import std::File;
import std::IO -> read, write;
```

### Advanced Module Features

```argonlang
// Dynamic module with hooks
module advanced = struct {
    SomeClass,
    __on_import = () -> {
        print("Module imported!");
        return 42;  // Return value available to importer
    },
    __guard = (info: ImportInfo) -> {
        if (info.version < "2.0") {
            return Error("Version 2.0+ required");
        }
        return Ok();
    }
};

// Usage
def result = import advanced;  // Prints "Module imported!", result = 42
```

## Advanced Features

### Unions and Enums

```argonlang
// Named union with fields
union Int {
    bit32: i32,
    bit64: i64
}

// Using unions with pattern matching
func get_rand() i32 | i64 {
    return 184828;
}

func main() {
    def num: Int = get_rand();
    num => {
        i32 -> print("Num is 32 bit: ", num),
        i64 -> print("Num is 64 bit: ", num)
    }
}

// Simple enum (with optional explicit values)
enum Color {
    Red,
    Green,
    Blue
}

// Structured enum with fields
enum Shape {
    Circle{ radius: i32 },
    Rectangle{ width: i32, height: i32 }
}

// Pattern matching on enums (note: curly braces for destructuring)
func area(shape: Shape) f32 {
    return shape => {
        Shape::Circle{ radius } -> 3.14 * radius * radius,
        Shape::Rectangle{ width, height } -> width * height
    };
}
```

**Key Differences:**
- **Unions**: Pure type descriptors, use field names with types
- **Enums**: Declared constructors, create actual values (e.g., `Shape::Circle` exists)

### Advanced Enum Features

```argonlang
// Enum with constraint - all variants must implement a class
class Student pub {
    name: str;
    grade: u32;
}

enum Students -> Student {
    George("George", 100),  // Uses constructor
    Anna{ grade = 200, name = "Anna" }  // Uses field initialization
}

// Enum with explicit values
enum Status {
    Pending = 0,
    InProgress = 1,
    Complete = 2,
    Failed = -1
}
```

### Type Aliases

```argonlang
// Simple alias
using Matrix = vec<vec<f64>>;
using UserId = i64;
using Callback = func(i32) void;

// Generic alias
using Optional<T> = Maybe<T>;
using Result<T> = Either<T, E>;
```

### Operator Overloading

```argonlang
class Vector {
    x: f64;
    y: f64;
    
    // Arithmetic operators
    func _add(other: Vector) Vector {
        return Vector(x + other.x, y + other.y);
    }
    
    func _sub(other: Vector) Vector {
        return Vector(x - other.x, y - other.y);
    }
    
    // Comparison operators
    func _eq(other: Vector) bool {
        return x == other.x && y == other.y;
    }
    
    // Custom behavior
    func _get() str {  // Called when value is accessed
        return "Vector(" + x + ", " + y + ")";
    }
}

// Usage
def v1 = Vector(1.0, 2.0);
def v2 = Vector(3.0, 4.0);
def sum = v1 + v2;  // Calls v1._add(v2)
print(v1);          // Calls v1._get()
```

### Macros and Metaprogramming

```argonlang
// Simple macros
@def MAX_SIZE = 100;
@func square(x) -> x * x;

// Function-like macros
@func repeat(action: Function, count: Number) {
    0 to count ||> action;
}

@repeat((i) -> print(i), 5);

// Conditional compilation
@if (DEBUG) {
    func debugPrint(msg: str) { print("[DEBUG] " + msg); }
} @else {
    func debugPrint(msg: str) { /* no-op */ }
}

// Code generation
@func createClass(base: id, fields: vec<struct{base: str, type: str}>) {
    class base {
        fields.each(field -> field.base: field.type;);
    }
}

@createClass(User, [
    struct{base = "id", type = "i32"},
    struct{base = "name", type = "str"}
]);
```

## Standard Library

### Core Libraries

```argonlang
// File I/O
import std::File;

def content = File::read("data.txt");
File::write("output.txt", "Hello, World!");

// Input/Output
import std::IO;

def userInput = IO::readLine();
IO::print("Enter a number: ");
def number = IO::readInt();

// Mathematics
import std::Math;

def result = Math::sqrt(16.0);      // 4.0
def power = Math::pow(2.0, 3.0);    // 8.0
def angle = Math::sin(Math::PI / 2); // 1.0

// Collections
import std::Collections;

def set = Set<i32>();
set.insert(1);
set.insert(2);

def map = Map<str, i32>();
map["key"] = 42;
```

### Data Structures

```argonlang
// Available containers
import std::Containers;

def vector = Vector<i32>();       // Dynamic array
def list = LinkedList<i32>();     // Doubly-linked list
def deque = Deque<i32>();         // Double-ended queue
def stack = Stack<i32>();         // LIFO stack
def queue = Queue<i32>();         // FIFO queue
def set = HashSet<i32>();         // Hash set
def map = HashMap<str, i32>();    // Hash map
def tree = BTreeMap<i32, str>();  // Ordered map
```

### Algorithms

```argonlang
import std::Algorithms;

def numbers = [3, 1, 4, 1, 5, 9, 2, 6];

// Sorting
Algorithms::sort(numbers);
Algorithms::stableSort(numbers);

// Searching
def found = Algorithms::binarySearch(numbers, 5);
def pos = Algorithms::find(numbers, 1);

// Transformations
def doubled = Algorithms::transform(numbers, x -> x * 2);
def evens = Algorithms::filter(numbers, x -> x % 2 == 0);
def sum = Algorithms::reduce(numbers, 0, (acc, x) -> acc + x);
```

## Debugging and Testing

### Debug Features

```argonlang
#debug;  // Enable debug mode

// Logging
#log("Debug message");
#log(variable);  // Print variable value and type

// Function tracing
#trace myFunction;

// Breakpoints
func complexCalculation() {
    for (i -> 0 to 100) {
        #breakpoint;  // Pause execution
        process(i);
    }
}

// Variable watching
#watch globalVariable;

// Performance profiling
#profile(expensiveFunction);
```

### Testing Framework

```argonlang
// Unit tests
#test factorial(result) {
    (0) -> result == 1,
    (1) -> result == 1,
    (5) -> result == 120,
    (10) -> result == 3628800
}

// Property-based testing
#test add(result)[a = -100 to 100, b = -100 to 100] {
    (a, b) -> result == a + b,
    (0, b) -> result == b,
    (a, 0) -> result == a
}

// Assertions
#assert factorial(5) == 120;
#assert "Testing string equality" == "Testing string equality";
```

### Visualization and Analysis

```argonlang
// Data structure visualization
def tree = createBinaryTree();
#visualize(tree);  // Generates .dot file

// Execution timeline
#timeline;
func main() {
    task1();
    par task2();
    par task3();
    task4();
}
// Generates timeline showing parallel execution
```

## FFI (Foreign Function Interface)

### C/C++ Integration

```argonlang
// External function declarations
@extern func printf(format: str, ...) i32;

// Library linking
@link("math");
@extern func sqrt(x: f64) f64;

// Direct C++ code injection
@cpp {
    std::cout << "Direct C++ code" << std::endl;
}

// Inline assembly
@asm {
    mov eax, 1
    int 0x80
}

// Header file generation
@header;  // Mark for header file inclusion
```

## Best Practices

### Code Organization

1. **Use meaningful variable and function names**
   ```argonlang
   // Good
   def userAge: i32 = 25;
   func calculateTotalPrice(items: vec<Item>) f64;
   
   // Avoid
   def x: i32 = 25;
   func calc(items: vec<Item>) f64;
   ```

2. **Leverage type safety and constraints**
   ```argonlang
   // Use constraints to encode invariants
   func divide<T: Number & NonZero>(a: T, b: T) T {
       return a / b;  // Safe by construction
   }
   ```

3. **Prefer immutability when possible**
   ```argonlang
   // Prefer const for values that don't change
   const CONFIG_FILE: str = "config.json";
   
   // Use def for mutable state only when necessary
   def counter: i32 = 0;
   ```

### Performance Tips

1. **Use appropriate collection types**
   ```argonlang
   // Use vec<T> for random access
   def scores: vec<i32> = [95, 87, 92];
   
   // Use list<T> for frequent insertion/deletion
   def tasks: list<Task> = [];
   ```

2. **Leverage lazy evaluation with ranges**
   ```argonlang
   // Lazy - computed on demand
   def numbers = range<i32>(0) | x -> x % 2 == 0;
   
   // Eager - computed immediately
   def numbers: vec<i32> = (0 to 1000) | x -> x % 2 == 0;
   ```

3. **Use move semantics for expensive objects**
   ```argonlang
   func processLargeData(data: ~LargeObject) {
       // Takes ownership, no copying
   }
   ```

### Safety Guidelines

1. **Always handle potential errors**
   ```argonlang
   // Good - explicit error handling
   def result = divide(a, b) catch (e) -> 0;
   
   // Better - propagate errors
   def result = try divide(a, b);
   ```

2. **Use references instead of raw pointers**
   ```argonlang
   // Preferred
   func process(data: &LargeObject) { }
   
   // Avoid unless necessary
   func process(data: *LargeObject) { }
   ```

3. **Validate inputs with constraints**
   ```argonlang
   func processAge<T: Number & InRange<T, 0, 150>>(age: T) {
       // age is guaranteed to be valid
   }
   ```

---

**ArgonLang** combines modern language design with practical performance requirements, making it suitable for systems programming, application development, and high-performance computing. Its rich type system, memory safety features, and expressive syntax enable developers to write both safe and efficient code.

---

## Summary

This documentation covers ArgonLang's core features with original examples demonstrating:

- **Advanced pattern matching** with destructuring and guards  
- **Functional programming** with pipeline operators and composition
- **Memory ownership** without complexity
- **Concurrent programming** with par/await and generators
- **Type safety** through value-set constraints
- **Error handling** with monadic types and pattern matching

The remaining sections would cover:

### Object System
- Class definitions with automatic field mapping
- Interface implementation and multiple inheritance
- Inline struct literals and anonymous objects
- Method resolution and static dispatch

### Memory Ownership  
- Owned pointers (~), references (&, &&), and raw pointers (*)
- Reference-extended lifetimes: values stay allocated while references exist
- Automatic lifetime management without annotations
- Custom allocators (heap, stack buffer, arena)
- Borrowing rules and compile-time safety

### Concurrent Programming
- `par` expressions for parallel execution
- `yield` for generator functions  
- Scoped parallelism with automatic synchronization
- Thread management and synchronization primitives

### Type Safety & Constraints
- Generic functions and classes with constraints
- Value-set based type system
- Compile-time contract verification
- C++20 concepts integration

### Error Management
- Maybe/Either monads for safe error handling
- Pattern matching on error types
- Exception propagation with `try`/`catch`/`throws`
- Contract-based programming

### Module Architecture
- Module definition and selective exports
- Import strategies (whole, selective, using)
- Dynamic modules with hooks and guards
- Standard library organization

### Metaprogramming
- Compile-time macros and code generation
- Conditional compilation
- Type introspection and reflection
- Custom operators and DSL creation

### Development Tools
- Debug logging and tracing
- Unit testing framework with property-based testing
- Performance profiling and visualization
- Memory safety analysis

ArgonLang represents a new paradigm in systems programming, combining the best aspects of functional and imperative programming while maintaining zero-cost abstractions and compile-time safety guarantees.

For complete examples and implementation details, see the [examples/](../examples/) directory.

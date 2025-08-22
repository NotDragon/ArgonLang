# ArgonLang Programming Language

**ArgonLang** is a modern, high-performance systems programming language that combines the safety and expressiveness of Rust with the performance of C++. It features advanced type safety, functional programming constructs, memory safety, and compile-time verification.

## Table of Contents

- [Getting Started](#getting-started)
- [Basic Syntax](#basic-syntax)
- [Variables and Types](#variables-and-types)
- [Functions](#functions)
- [Control Flow](#control-flow)
- [Collections and Ranges](#collections-and-ranges)
- [Functional Programming](#functional-programming)
- [Pattern Matching](#pattern-matching)
- [Classes and Objects](#classes-and-objects)
- [Memory Management](#memory-management)
- [Generics and Constraints](#generics-and-constraints)
- [Concurrency and Parallelism](#concurrency-and-parallelism)
- [Error Handling](#error-handling)
- [Modules and Imports](#modules-and-imports)
- [Advanced Features](#advanced-features)
- [Standard Library](#standard-library)
- [Debugging and Testing](#debugging-and-testing)

## Getting Started

### Hello World

```argonlang
func main() i32 {
    print("Hello, World!");
    return 0;
}
```

### Compilation

```bash
argonc main.arg -o main
./main
```

## Basic Syntax

### Comments

```argonlang
// Single line comment

/*
  Multi-line comment
  block
*/
```

### Semicolons

Semicolons are required to terminate statements:

```argonlang
def x = 10;
print(x);
```

## Variables and Types

### Variable Declarations

```argonlang
// Mutable variables
def i: i32 = 10;        // 32-bit integer
def f: f32 = 3.14;      // 32-bit float
def name: str = "Alice"; // String

// Immutable variables (constants)
const PI: f32 = 3.14159;
const MAX_SIZE: i32 = 100;

// Type inference
def x = 42;             // Inferred as i32
def message = "Hello";  // Inferred as str
```

### Primitive Types

| Type | Description | Example |
|------|-------------|---------|
| `i8`, `i16`, `i32`, `i64` | Signed integers | `def x: i32 = -42;` |
| `u8`, `u16`, `u32`, `u64` | Unsigned integers | `def x: u32 = 42;` |
| `f32`, `f64` | Floating-point | `def x: f64 = 3.14;` |
| `bool` | Boolean | `def x: bool = true;` |
| `char` | Character | `def x: char = 'A';` |
| `str` | String | `def x: str = "Hello";` |

### Arrays and Collections

```argonlang
// Fixed-size arrays
def arr: i32[10];                    // Array of 10 integers
def numbers: i32[5] = [1, 2, 3, 4, 5];

// Dynamic vectors
def vec: vec<i32>(10);               // Vector with initial capacity
def dynamic = vec<i32>();            // Empty vector

// Lists (linked lists)
def list: list<i32>(10);             // Linked list

// Ranges (lazy sequences)
def range: range<i32>(0, 10);        // Values between 0 and 10
```

## Functions

### Basic Function Declaration

```argonlang
// Function declaration
func add(a: i32, b: i32) i32;

// Function definition
func add(a: i32, b: i32) i32 {
    return a + b;
}

// Inline function (expression-based)
func double(x: i32) i32 -> x * 2;

// Function with no return value
func greet(name: str) {
    print("Hello, " + name + "!");
}
```

### Advanced Function Features

```argonlang
// Variadic functions
func print(text: str, ...args: vec<any>) {
    // Implementation
}

// Higher-order functions
func apply(fn: func(i32) i32, value: i32) i32 {
    return fn(value);
}

// Closures
func makeCounter() func() i32 {
    def count = 0;
    return () -> ++count;
}

// Pattern matching functions
func factorial(n: i32) => i32 {
    0 -> 1,
    _ -> n * factorial(n - 1)
}
```

### Reference Parameters

```argonlang
func half(n: i32) i32 -> n / 2;           // Pass by value
func halfRef(n: &i32) i32 -> n / 2;       // Immutable reference
func halfMut(n: &&i32) i32 -> n /= 2;     // Mutable reference

def x = 10;
half(x);        // x remains 10
halfRef(&x);    // x remains 10
halfMut(&&x);   // x becomes 5
```

## Control Flow

### Conditional Statements

```argonlang
// Basic if-else
if (condition) {
    // code
} else if (other_condition) {
    // code
} else {
    // code
}

// Branched values with type refinement
def value = getUserInput();
if (value > 0) [positiveValue] {
    // positiveValue is guaranteed to be > 0
    processPositive(positiveValue);
}
```

### Loops

```argonlang
// Range-based for loop
for (def i -> 0 to 10) {
    print(i);
}

// Inclusive range
for (def i -> 0 to= 10) {
    print(i);  // Prints 0 through 10
}

// Iterator-based loops
for (def item -> $array) {
    print(item);
}

// Collection loops
for (def num -> numbers) {
    print(num);
}

// While loops
while (condition) {
    // code
}
```

## Collections and Ranges

### Range Operations

```argonlang
// Basic ranges
def arr = 0 to 10;                    // [0, 1, 2, ..., 9]
def inclusive = 0 to= 10;             // [0, 1, 2, ..., 10]

// Range with filtering
def evens = 0 to 10 | x -> x % 2 == 0;  // [0, 2, 4, 6, 8]

// Range with mapping
def doubled = 0 to 10 & x -> x * 2;     // [0, 2, 4, ..., 18]

// Converting to concrete types
def vector: vec<i32> = 0 to 100;
```

### Array Operations

```argonlang
// Indexing
def first = arr[0];
def slice = arr[2:4];              // Elements 2 and 3
def multiple = arr[0, 3, 5];       // Elements at indices 0, 3, 5

// Destructuring
def [first, second], rest = 0 to 10;
def [head], tail = someArray;
def remaining, [last] = someArray;
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

// Reduce operator ^
def sum = numbers ^ (acc, current) -> acc + current;

// Pipe operator |>
def result = numbers
    |> filter(x -> x > 5)
    |> map(x -> x * 2)
    |> sum();

// Map-pipe operator ||>
def processed = matrix ||> sum |> max;  // Map sum over matrix, then find max

// Accumulate operator ^^
def cumulative = numbers ^^ (acc, current) -> acc + current;
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
    def name: str;
    def grade: i32;
    
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
    def width: i32;
    def height: i32;
    
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
    def value: i32 = 0;
    func increment() -> value++;
}
```

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
    def color: str;
}

class ColoredCircle impl Drawable, Shape {
    def radius: i32;
    
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

### Pointer Types

```argonlang
// Raw pointers (C-style)
def ptr: *i32;           // Raw pointer
def arr: *i32[5];        // Pointer to array

// Owned pointers (automatic cleanup)
def owned: ~i32 = 42;    // Owned pointer

// Reference counting
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

### Memory Safety

```argonlang
// Ownership rules enforced at compile time
func example() {
    def data: ~i32 = 42;
    def ref1 = &data;     // Immutable reference
    def ref2 = &data;     // Multiple immutable refs OK
    
    // def mutRef = &&data;  // Error: cannot have mutable ref with immutable refs
}

// Mutability tracking
class Container {
    def mut data: vec<i32>;  // Mutable field
    
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
    def data: T;
    
    pub constructor(data: T) { }
    
    pub func get() T {
        return data;
    }
}

// Constrained generic class
class NumericContainer<T: Number> {
    def value: T;
    
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
func findElement(arr: vec<i32>, target: i32) Maybe<i32> {
    for (i, value -> arr.enumerate()) {
        if (value == target) {
            return Maybe::Some(i);
        }
    }
    return Maybe::None;
}

// Either type for error handling
func safeDivide(a: i32, b: i32) Either<i32, str> {
    if (b == 0) {
        return Either::Right("Division by zero");
    }
    return Either::Left(a / b);
}

// Pattern matching on results
def result = safeDivide(10, 2) => {
    Either::Left(value) -> "Result: " + value,
    Either::Right(error) -> "Error: " + error
};
```

### Contracts and Preconditions

```argonlang
// Function contracts
func divide(a: i32, b: i32) [expects: b != 0, ensures: result == a / b] i32 {
    return a / b;
}

// Usage with contract checking
def result = try divide(10, getUserInput());  // Safe: wrapped in try
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
using import std::Math;  // Import and use
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
// Simple union
union Number = i32 | f64;
def value: Number = 42;  // Can be either i32 or f64

// Named union
union Result {
    success: i32,
    error: str
}

// Enum
enum Color {
    Red,
    Green,
    Blue
}

// Tagged union (sum type)
enum union Shape {
    Circle(radius: f64),
    Rectangle(width: f64, height: f64),
    Triangle(a: f64, b: f64, c: f64)
}

// Pattern matching on unions
def area = (shape: Shape) => f64 {
    Shape::Circle(r) -> PI * r * r,
    Shape::Rectangle(w, h) -> w * h,
    Shape::Triangle(a, b, c) -> {
        def s = (a + b + c) / 2;
        return Math::sqrt(s * (s - a) * (s - b) * (s - c));
    }
};
```

### Type Aliases

```argonlang
// Simple alias
using Matrix = vec<vec<f64>>;
using UserId = i64;
using Callback = func(i32) void;

// Generic alias
using Optional<T> = Maybe<T>;
using Result<T, E> = Either<T, E>;
```

### Operator Overloading

```argonlang
class Vector {
    def x: f64;
    def y: f64;
    
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
@func createClass(name: id, fields: vec<struct{name: str, type: str}>) {
    class name {
        fields.each(field -> def field.name: field.type;);
    }
}

@createClass(User, [
    struct{name = "id", type = "i64"},
    struct{name = "name", type = "str"}
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
#debug  // Enable debug mode

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
@link("math")
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
@header  // Mark for header file inclusion
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

For more examples and detailed documentation, visit the [ArgonLang Examples](../examples/) directory.

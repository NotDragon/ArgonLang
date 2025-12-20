# ArgonLang

ArgonLang is a modern, statically-typed programming language designed to be used as an alternative to C++. it compiles down to C++, allowing for the two languages to be used together.

## Key Features

- **Static Typing**: A strong type system that ensures type safety at compile time.
- **Readable Syntax**: Write clean, maintainable code.
- **C++ Compatability**: All C++ code can be run with ArgonLang.

## Hello, World!

Here is a simple "Hello, World!" program in ArgonLang:

```argon
import io;

func main() {
    print("Hello, World!");
}
```

Compile and run it:
```bash
argon hello.arg -o hello
./hello
```

## Implementation Status

This is an actively developed language. Below is a comprehensive checklist of planned features and their implementation status:

#### Statements & Control Flow
- [x] Variable declarations (`def`, `const`)
- [x] Function declarations
- [x] If statements
- [x] While loops
- [x] For loops
- [x] Return statements
- [x] Break/Continue
- [x] Block statements
- [x] Yield statements (parsing only)

#### Expressions
- [x] Literals (integers, floats, strings, chars, booleans, null)
- [x] Binary operations (arithmetic, logical, bitwise)
- [x] Unary operations (increment, decrement, negation, not)
- [x] Assignment expressions
- [x] Ternary operator (`? :`)
- [x] Function calls
- [x] Lambda expressions
- [x] Member access (`.` operator)
- [x] Array indexing
- [x] Array slicing
- [x] Multiple indexing
- [x] Range expressions (`to`, `to=`)
- [x] Struct literals
- [x] Match expressions (parsing only)
- [x] Parallel expressions (`par`)

#### Type System
- [x] Primitive types (i8, i16, i32, i64, i128, f32, f64, f128)
- [x] Identifier types
- [x] Generic types (`vec<i32>`)
- [x] Function types (`func(i32) i32`)
- [x] Array types (`i32[10]`)
- [x] Sum types (union `|`)
- [x] Intersection types (`&`)
- [x] Variadic types (`...T`)
- [x] Type aliases (`using`)

#### Pattern Matching
- [x] Wildcard patterns (`_`)
- [x] Literal patterns
- [x] Identifier patterns
- [x] Array patterns with destructuring
- [x] Struct patterns
- [x] Constructor patterns
- [x] Type patterns
- [x] Range patterns

#### Object-Oriented Features
- [x] Class declarations
- [x] Constructor statements
- [x] Impl blocks
- [x] Member visibility (`pub`, `pri`, `pro`)
- [x] Generic classes (parsing)

#### Advanced Declarations
- [x] Enum declarations
- [x] Union declarations
- [x] Constraint declarations (parsing only)
- [x] Module declarations (parsing only)
- [x] Import statements (parsing only)

#### Operators (Tokenization)
- [x] Filter operator (`|`)
- [x] Map operator (`&`)
- [x] Reduce operator (`?`)
- [x] Pipe operator (`|>`)
- [x] Assignment variants (`+=`, `-=`, `*=`, `/=`, `%=`, `|=`, `&=`, `?=`, `|>=`)

#### Type Checking & Analysis
- [x] Basic type checking
- [ ] Generic constraint verification
- [ ] Full type inference
- [ ] Constraint satisfaction checking

#### Code Generation
- [x] Basic C++ code generation
- [ ] Full pattern matching code generation
- [ ] Generator functions (yield)
- [ ] Parallel execution code generation

#### Memory Management
- [ ] Owned pointers (`~T`)
- [ ] Reference counting (`ref<T>`, `weak<T>`)
- [ ] Mutable references (`&&T`)
- [ ] Immutable references (`&T`)
- [ ] Raw pointer arrays (`*T[N]`)
- [ ] Manual memory management (`delete`, `delete[]`)
- [ ] Mutability tracking (`mut` keyword)
- [ ] Custom allocators (`alloc::heap`, `alloc::buffer`, `alloc::bump`)
- [ ] Function-level allocators (`use` keyword)

#### Functional Programming
- [ ] Partial application with underscore (`add(10, _)`)
- [ ] Runtime evaluation of functional operators
- [ ] Destructuring assignment (`def [a, b] = arr`)
- [ ] Struct destructuring (`def { x, y } = point`)

#### Advanced Type System
- [ ] Range-constrained types (`range<i32>(0, 10)`)
- [ ] Compile-time traits (`i32 & Positive`)
- [ ] Function contracts (`[expects:, ensures:]`)
- [ ] Flow-sensitive typing (`if(x > 0) [positive] { }`)
- [ ] Type checking operators (`is`, `typeof`)

#### Object-Oriented
- [ ] Interfaces (`inter` keyword)
- [ ] Class inheritance (`impl Shape`)
- [ ] Inline objects with interfaces
- [ ] Nested enums in classes
- [ ] Operation overloading (`_add`, `_get`, `_set`, etc.)
- [ ] Multiple named constructors
- [ ] External impl blocks (`impl Student func ...`)
- [ ] Const classes

#### Concurrency & Async
- [ ] Async par with configuration
- [ ] Future callbacks (`onYield`, `onReturn`, `onError`, etc.)
- [ ] Future combinators (`whenAll`, `whenAny`)
- [ ] Future control (`kill`, `await`)
- [ ] Thread management (`Thread` class)
- [ ] Lock sugar (`lock(mutex) { }`)
- [ ] Scoped parallelism with automatic synchronization

#### Error Handling
- [ ] Try-catch syntax (`try`, `catch`, `throws`)
- [ ] Maybe monad (`Maybe<T>`)
- [ ] Either monad (`Either<T, E>`)
- [ ] Error enums with enum unions
- [ ] Error propagation

#### Advanced Language Features
- [ ] Variadic parameter implementation
- [ ] Iterator operator (`$arr`)
- [ ] Pure functions (`const func`)
- [ ] Lambda sugar for blocks
- [ ] Auxiliary logic with `where`
- [ ] Named parameters/labels (`@param`)
- [ ] Decorators (`[[decorator]]`)

#### Metaprogramming
- [ ] Preprocessor macros (`@def`, `@func`)
- [ ] Conditional compilation (`@if`, `@else`)
- [ ] Scope cleanup (`@cleanup`)
- [ ] Code generation macros
- [ ] Compile-time contracts (`@ensure`)

#### Debugging & Testing
- [ ] Debug mode (`#debug`)
- [ ] Logging (`#log`)
- [ ] Breakpoints (`#breakpoint`)
- [ ] Variable watching (`#watch`)
- [ ] Performance profiling (`#profile`)
- [ ] Call tracing (`#trace`)
- [ ] Data structure visualization (`#visualize`)
- [ ] Execution timeline (`#timeline`)
- [ ] Testing framework (`#test`, `#assert`)
- [ ] Property-based testing

#### FFI (Foreign Function Interface)
- [ ] External function declarations (`@extern`)
- [ ] Library linking (`@link`)
- [ ] Inline C++ code (`@cpp`)
- [ ] Inline assembly (`@asm`)
- [ ] Header file generation (`@header`)

#### Module System (Runtime)
- [ ] Module export restrictions
- [ ] Scoped imports
- [ ] Module hooks (`__on_import`, `__guard`)
- [ ] Using directive implementation

#### Standard Library
- [ ] File I/O
- [ ] Console I/O
- [ ] Math functions
- [ ] Collections (Vec, List, Set, Map, etc.)
- [ ] Algorithms (sort, search, transform, etc.)
- [ ] String utilities
- [ ] Date/Time utilities

#### Build System
- [ ] Build configuration files (`build.arg`)
- [ ] Package manager
- [ ] Dependency resolution

## Building

```bash
git clone https://github.com/NotDragon/ArgonLang.git
cd ArgonLang
mkdir build
cd build
cmake -G "MinGW Makefiles" .. # or "Unix Makefiles"
mingw32-make # or make
```

## Documentation

For detailed language documentation and examples, see:
- [Language Specification](syntax.txt) - Complete syntax reference
- [Documentation](docs/README.md) - Comprehensive language guide
- [Examples](examples/) - Example programs

## Contributing

ArgonLang is under active development. Contributions are welcome! Please check the implementation status above to see what features need work.

## License

ArgonLang is licensed under the Apache License, Version 2.0 License. See the [LICENSE](LICENSE) file for details.

---

Happy coding with ArgonLang!

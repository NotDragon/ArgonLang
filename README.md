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

## Syntax Overview
Here is some sample syntax
### Variables

#### Variable Declarations
```argon
const x: i32 = 42;    // Immutable variable
def y: f64 = 3.14;  // Mutable variable
```

#### Arrays
```argon
def arr: i32[10]; // Fixed-Sized array
def v: vec<i32>; // Dynamic array
```

### Functions

#### Function Definitions
```argon
func add(a: i32, b:i32); // Function Definition

func add(a: i32, b: i32) -> i32 { // Funciton Implamentation
    return a + b;
}
```

#### Inline Functions
```argon
func subtract(a: i32, b: i32) -> a - b;
```

#### Lambda Functions
```argon
def add = (x, y) -> x + y;
```

#### Lambda Types
```argon
def getRand: func() -> i32 = rand; // getRand expects no arguments, and returns an i32
```

#### Closures

```argon
func closure() -> func -> i32 {
	def count = 0;
	return () -> ++count;
}
```

### Expressions

#### To Expression
```argon
0 to 10
0 to= 10
```
Here the `0 to 10` will return an array of number 0 up 10, excluding 10.
`0 to=10` will return an array of number 0 up 10, including 10.

#### Filter Expression
```argon 
def newArr = arr | x: i32 -> x % 2 == 0;
```
Here all values inside arr that do not meet the condition x % 2 == 0, will be filtered

#### Map Expression
```argon 
def doubled = arr & x: i32 -> x * 2;
```

#### Reduce Expression
```argon 
def sum = arr ^ (x, index, previous) -> i * p;
```

### Control Flow

#### If Statement
```argon
if (condition) {
    print("Condition is true");
} else {
    print("Condition is false");
}
```

#### While Statement
```argon
while(condition) {
   print("Running");
} else {
   print("Condition is false");
}
```

#### For Loops
```argon
for (i -> 0 to 10) {
    print(i);
}
```

### Advanced Types

```argon
def bigNumber: i128 = 170141183460469231731687303715884105727;
```
## License

ArgonLang is licensed under the Apache License, Version 2.0 License. See the [LICENSE](LICENSE) file for details.

---

Happy coding with ArgonLang!

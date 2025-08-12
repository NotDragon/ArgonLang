# ArgonLang Examples

This directory contains various ArgonLang code examples for testing the compiler and demonstrating language features.

## Simple Examples (Basic Functionality)

- `00_minimal.arg` - Minimal working program with just main function
- `00_simple_math.arg` - Basic arithmetic operations
- `01_basic_variables.arg` - Variable declarations and basic types
- `02_simple_functions.arg` - Function declarations and definitions
- `03_control_flow.arg` - If statements, loops, and control structures

## Intermediate Examples

- `04_classes_basic.arg` - Basic class declarations and usage
- `05_type_aliases_unions.arg` - Type aliases, unions, and enums
- `06_functional_programming.arg` - Function composition and higher-order functions (simplified)

## Advanced Examples

- `07_pattern_matching.arg` - Pattern matching and match expressions
- `08_memory_management.arg` - Pointers, references, and ownership
- `09_traits_generics.arg` - Traits and generic programming
- `10_modules_imports.arg` - Module system and imports
- `11_error_handling.arg` - Error handling patterns
- `12_comprehensive_test.arg` - Comprehensive test combining multiple features

## Usage

To test these examples with the ArgonLang compiler:

```bash
# Build the compiler first
cmake --build build

# Run an example
./build/ArgonLang.exe ./examples/00_minimal.arg ast.dot output.cpp

# Or test multiple examples
for file in examples/*.arg; do
    echo "Testing $file"
    ./build/ArgonLang.exe "$file" "ast_$(basename $file .arg).dot" "out_$(basename $file .arg).cpp"
done
```

## Notes

- Examples are ordered roughly by complexity
- Start with `00_minimal.arg` to test basic compilation
- Some advanced features may not be fully implemented yet
- Examples are designed to test different aspects of the language
- Each example focuses on specific language features for targeted testing

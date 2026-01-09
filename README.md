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

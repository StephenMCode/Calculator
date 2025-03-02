# C++ Calculator

A comprehensive calculator application written in C++ that performs both basic arithmetic and scientific operations.

## Features

- Basic arithmetic operations:
  - Addition (+)
  - Subtraction (-)
  - Multiplication (*)
  - Division (/)
  
- Scientific operations:
  - Square Root (s)
  - Power/Exponentiation (p)
  - Logarithm with custom base (l)
  - Natural Logarithm (n)

- User-friendly command-line interface
- Robust error handling for:
  - Invalid operations
  - Invalid inputs
  - Division by zero
  - Negative square roots
  - Invalid logarithm arguments

## Requirements

- C++ compiler (supporting C++11 or later)
- CMake (optional, for building)

## Building the Project

### Using g++ directly

```bash
g++ -o calculator main.cpp calculator.cpp -std=c++11
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./calculator
```

Follow the on-screen instructions to perform calculations:

1. Select an operation from the menu
2. Enter the required number(s)
3. View the result
4. Continue with another calculation or quit

### Example Operations

- Square Root: `sqrt(25) = 5`
- Power: `2^3 = 8`
- Logarithm: `log_10(100) = 2`
- Natural Logarithm: `ln(2.71828) ≈ 1`

## Future Enhancements

Planned features for future versions:
- Memory functions (store, recall, clear)
- Trigonometric functions (sin, cos, tan)
- Percentage calculations
- Unit conversions
- Graphical user interface

## License

This project is open source and available under the [MIT License](LICENSE). 
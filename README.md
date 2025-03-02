# C++ Calculator

A comprehensive calculator application written in C++ with a graphical user interface using the Windows API.

## Features

- Basic arithmetic operations:
  - Addition (+)
  - Subtraction (-)
  - Multiplication (x)
  - Division (/)
  
- Scientific operations:
  - Square Root (s)
  - Power/Exponentiation (^)
  - Logarithm with custom base (log)
  - Natural Logarithm (ln)

- User-friendly graphical interface with:
  - Numeric keypad (0-9)
  - Decimal point (.)
  - Operation buttons
  - Clear button (C)
  - About dialog
  - Operation display showing current calculation

- Robust error handling for:
  - Invalid operations
  - Invalid inputs
  - Division by zero
  - Negative square roots
  - Invalid logarithm arguments

## Requirements

- Windows operating system
- g++ compiler (supporting C++11 or later)
- windres (for compiling resources)

## Building the Project

Use the provided compile.bat script:

```bash
compile.bat
```

Or compile manually:

```bash
windres calculator.rc -O coff -o calculator.res
g++ -std=c++11 -mwindows main.cpp calculator.cpp calculator.res -o calculator.exe
```

## Running the Application

Use the provided run.bat script:

```bash
run.bat
```

Or run directly:

```bash
calculator.exe
```

## Usage

The graphical interface provides:
- Numeric keypad (0-9)
- Decimal point (.)
- Operation buttons (+, -, x, /)
- Scientific operation buttons (s, ^, ln, log)
- Clear button (C)
- Equals button (=)
- About button
- Operation display showing the current operation and first operand

### Example Operations

- Square Root: s(25) = 5
- Power: 2^3 = 8
- Logarithm: log_10(100) = 2
- Natural Logarithm: ln(2.71828) ≈ 1

## License

This project is open source and available under the [MIT License](LICENSE). 
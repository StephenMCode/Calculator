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

- Memory functions:
  - Memory Add (M+)
  - Memory Subtract (M-)
  - Memory Recall (MR)
  - Memory Clear (MC)
  - Memory indicator showing when a value is stored

- User-friendly graphical interface with:
  - Numeric keypad (0-9)
  - Decimal point (.)
  - Operation buttons
  - Clear button (C)
  - About dialog
  - Operation display showing current calculation
  - History display showing previous calculations
  - Multiple theme options

- Customizable themes:
  - Default theme (Light)
  - Dark theme
  - Blue theme
  - Theme switching at runtime
  - Color-coded buttons by function

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
- Memory operation buttons (M+, M-, MR, MC)
- Clear button (C)
- Equals button (=)
- About button
- Theme button (cycles through available themes)
- Operation display showing the current operation and first operand
- Memory indicator showing when a value is stored in memory
- History display showing previous calculations in chronological order (newest at the top)

### Example Operations

- Square Root: s(25) = 5
- Power: 2^3 = 8
- Logarithm: log_10(100) = 2
- Natural Logarithm: ln(2.71828) ≈ 1

### Memory Operations

- M+: Add the current display value to memory
- M-: Subtract the current display value from memory
- MR: Recall the value stored in memory
- MC: Clear the memory
- Memory indicator (M): Appears when a value is stored in memory

### History Display

The calculator includes a history panel that:
- Displays all calculations performed in the current session
- Shows the most recent calculations at the top
- Includes both the expression and the result
- Allows scrolling through previous calculations
- Persists until the application is closed

### Theme System

The calculator features a customizable theme system that allows users to change the visual appearance:
- **Default Theme**: A light theme with soft pastel colors for different button types
- **Dark Theme**: A dark-mode theme with contrasting colors for comfortable use in low-light environments
- **Blue Theme**: A blue-tinted theme with a professional appearance

Each theme customizes:
- Window background
- Display background and text
- Button backgrounds and text colors (with different colors for different button types)
- History display background and text

To change themes, simply click the "Theme" button to cycle through available options.

## License

This project is open source and available under the [MIT License](LICENSE). 
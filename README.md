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

- Advanced expression parsing:
  - Support for complex mathematical expressions
  - Proper operator precedence handling
  - Nested parentheses support
  - Function arguments can contain full expressions
  - Visual indicators for unclosed parentheses
  - Automatic parentheses completion

- Trigonometric functions:
  - Sine (sin)
  - Cosine (cos)
  - Tangent (tan)
  - Inverse trigonometric functions (asin, acos, atan)

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

- Keyboard shortcuts for all operations:
  - Number keys and operators
  - Function keys for memory operations
  - Scientific operations via keyboard
  - See "Keyboard Shortcuts" section for details

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

### Keyboard Shortcuts

The calculator supports keyboard input for all operations, making it faster and more efficient to use:

| Key/Combination | Function |
|-----------------|----------|
| 0-9 | Number keys (regular keyboard and numpad) |
| . | Decimal point |
| + | Addition |
| - | Subtraction |
| * or x | Multiplication |
| / | Division |
| Enter | Calculate (=) |
| Esc | Clear (C) |
| S | Square root |
| ^ (Shift+6) | Power/Exponentiation |
| L | Natural logarithm (ln) |
| Shift+L | Base-10 logarithm (log) |
| M | Memory add (M+) |
| Shift+M | Memory subtract (M-) |
| Ctrl+M | Memory recall (MR) |
| Ctrl+Shift+M | Memory clear (MC) |

*Note: Keyboard shortcuts are also listed in the About dialog (click the About button)*

### Example Operations

- Basic arithmetic: 5+5 = 10
- Order of operations: 2+3*4 = 14 (multiplication before addition)
- Parentheses: (2+3)*4 = 20 (changes precedence)
- Square Root: sqrt(25) = 5
- Power: 2^3 = 8
- Logarithm: log(100) = 2
- Natural Logarithm: ln(2.71828) ≈ 1
- Complex expressions: sin(45)*cos(30)+tan(15) 
- Nested functions: log(sqrt(16)) = 0.602
- Function with expressions: cos(8*3) = -0.911
- Mixed operations: 5^2 + sqrt(16) - ln(10) = 25 + 4 - 2.302 = 26.698

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
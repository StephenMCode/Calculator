#include <iostream>
#include <string>
#include <limits>
#include "calculator.h"

void displayMenu() {
    std::cout << "\n===== C++ Calculator =====\n";
    std::cout << "Available operations:\n";
    std::cout << "  + : Addition\n";
    std::cout << "  - : Subtraction\n";
    std::cout << "  * : Multiplication\n";
    std::cout << "  / : Division\n";
    std::cout << "  s : Square Root\n";
    std::cout << "  p : Power (x^y)\n";
    std::cout << "  l : Logarithm (log_y(x))\n";
    std::cout << "  n : Natural Logarithm (ln(x))\n";
    std::cout << "  q : Quit\n";
    std::cout << "=========================\n";
}

int main() {
    Calculator calculator;
    char operation;
    double num1, num2;
    bool continueCalculation = true;

    std::cout << "Welcome to C++ Calculator!\n";

    while (continueCalculation) {
        displayMenu();
        
        // Get operation from user
        std::cout << "Enter operation (or 'q' to quit): ";
        std::cin >> operation;
        
        if (operation == 'q' || operation == 'Q') {
            continueCalculation = false;
            std::cout << "Thank you for using C++ Calculator. Goodbye!\n";
            continue;
        }
        
        if (!calculator.isValidOperation(operation)) {
            std::cout << "Error: Invalid operation. Please try again.\n";
            // Clear input buffer
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        
        // Get operands from user
        if (operation == 's' || operation == 'n') {
            // Operations that only need one number
            std::cout << "Enter number: ";
            while (!(std::cin >> num1)) {
                std::cout << "Error: Invalid input. Please enter a number: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            num2 = 0; // Not used for these operations
        } else {
            // Operations that need two numbers
            std::cout << "Enter first number: ";
            while (!(std::cin >> num1)) {
                std::cout << "Error: Invalid input. Please enter a number: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            
            std::cout << "Enter second number: ";
            while (!(std::cin >> num2)) {
                std::cout << "Error: Invalid input. Please enter a number: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        
        // Perform calculation
        try {
            double result = calculator.calculate(num1, num2, operation);
            
            // Display result with appropriate format based on operation
            if (operation == 's') {
                std::cout << "Result: sqrt(" << num1 << ") = " << result << std::endl;
            } else if (operation == 'n') {
                std::cout << "Result: ln(" << num1 << ") = " << result << std::endl;
            } else if (operation == 'p') {
                std::cout << "Result: " << num1 << "^" << num2 << " = " << result << std::endl;
            } else if (operation == 'l') {
                std::cout << "Result: log_" << num2 << "(" << num1 << ") = " << result << std::endl;
            } else {
                std::cout << "Result: " << num1 << " " << operation << " " << num2 << " = " << result << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        
        // Clear input buffer
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    return 0;
} 
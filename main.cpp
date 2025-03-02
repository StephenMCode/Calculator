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
        
        // Perform calculation
        try {
            double result = calculator.calculate(num1, num2, operation);
            std::cout << "Result: " << num1 << " " << operation << " " << num2 << " = " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        
        // Clear input buffer
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    return 0;
} 
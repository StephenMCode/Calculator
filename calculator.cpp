#include "calculator.h"
#include <stdexcept>

Calculator::Calculator() {
    // Constructor implementation (currently empty as no initialization is needed)
}

double Calculator::add(double a, double b) {
    return a + b;
}

double Calculator::subtract(double a, double b) {
    return a - b;
}

double Calculator::multiply(double a, double b) {
    return a * b;
}

double Calculator::divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("Division by zero is not allowed");
    }
    return a / b;
}

bool Calculator::isValidOperation(char operation) {
    return operation == '+' || operation == '-' || 
           operation == '*' || operation == '/';
}

double Calculator::calculate(double a, double b, char operation) {
    switch (operation) {
        case '+':
            return add(a, b);
        case '-':
            return subtract(a, b);
        case '*':
            return multiply(a, b);
        case '/':
            return divide(a, b);
        default:
            throw std::invalid_argument("Invalid operation");
    }
} 
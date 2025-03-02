#include "calculator.h"
#include <stdexcept>
#include <cmath>

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

double Calculator::squareRoot(double a) {
    if (a < 0) {
        throw std::invalid_argument("Cannot calculate square root of a negative number");
    }
    return std::sqrt(a);
}

double Calculator::power(double base, double exponent) {
    return std::pow(base, exponent);
}

double Calculator::logarithm(double a, double base) {
    if (a <= 0 || base <= 0 || base == 1) {
        throw std::invalid_argument("Invalid arguments for logarithm");
    }
    return std::log(a) / std::log(base);
}

double Calculator::naturalLogarithm(double a) {
    if (a <= 0) {
        throw std::invalid_argument("Cannot calculate logarithm of a non-positive number");
    }
    return std::log(a);
}

bool Calculator::isValidOperation(char operation) {
    return operation == '+' || operation == '-' || 
           operation == '*' || operation == '/' ||
           operation == 's' || operation == 'p' ||
           operation == 'l' || operation == 'n';
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
        case 's':
            return squareRoot(a);
        case 'p':
            return power(a, b);
        case 'l':
            return logarithm(a, b);
        case 'n':
            return naturalLogarithm(a);
        default:
            throw std::invalid_argument("Invalid operation");
    }
} 
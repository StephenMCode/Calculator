#include "calculator.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E 
#define M_E 2.71828182845904523536
#endif

Calculator::Calculator() {
    initializeFunctions();
}

void Calculator::initializeFunctions() {
    functions["sin"] = [this](double a) { return this->sine(a); };
    functions["cos"] = [this](double a) { return this->cosine(a); };
    functions["tan"] = [this](double a) { return this->tangent(a); };
    functions["asin"] = [this](double a) { return this->arcsine(a); };
    functions["acos"] = [this](double a) { return this->arccosine(a); };
    functions["atan"] = [this](double a) { return this->arctangent(a); };
    functions["sinh"] = [this](double a) { return this->sineH(a); };
    functions["cosh"] = [this](double a) { return this->cosineH(a); };
    functions["tanh"] = [this](double a) { return this->tangentH(a); };
    functions["sqrt"] = [this](double a) { return this->squareRoot(a); };
    functions["ln"] = [this](double a) { return this->naturalLogarithm(a); };
    functions["log"] = [this](double a) { return this->logarithm(a, 10.0); };
    functions["abs"] = [this](double a) { return this->absolute(a); };
    functions["fact"] = [this](double a) { return this->factorial(a); };
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
    if (base < 0 && std::floor(exponent) == exponent) {
        if (std::fmod(exponent, 2.0) == 0.0) {
            return std::pow(-base, exponent);
        } else {
            return -std::pow(-base, exponent);
        }
    }
    
    if (base < 0 && std::floor(exponent) != exponent) {
        throw std::invalid_argument("Cannot raise negative number to non-integer power");
    }
    
    if (base == 0 && exponent < 0) {
        throw std::invalid_argument("Cannot raise zero to negative power");
    }
    
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

double Calculator::degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double Calculator::radiansToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

double Calculator::sine(double a, bool inDegrees) {
    if (inDegrees) {
        a = degreesToRadians(a);
    }
    return std::sin(a);
}

double Calculator::cosine(double a, bool inDegrees) {
    if (inDegrees) {
        a = degreesToRadians(a);
    }
    return std::cos(a);
}

double Calculator::tangent(double a, bool inDegrees) {
    if (inDegrees) {
        a = degreesToRadians(a);
    }
    if (std::abs(std::cos(a)) < 1e-10) {
        throw std::invalid_argument("Tangent is undefined at this angle");
    }
    return std::tan(a);
}

double Calculator::arcsine(double a, bool returnDegrees) {
    if (a < -1 || a > 1) {
        throw std::invalid_argument("Arcsine argument must be between -1 and 1");
    }
    double result = std::asin(a);
    if (returnDegrees) {
        result = radiansToDegrees(result);
    }
    return result;
}

double Calculator::arccosine(double a, bool returnDegrees) {
    if (a < -1 || a > 1) {
        throw std::invalid_argument("Arccosine argument must be between -1 and 1");
    }
    double result = std::acos(a);
    if (returnDegrees) {
        result = radiansToDegrees(result);
    }
    return result;
}

double Calculator::arctangent(double a, bool returnDegrees) {
    double result = std::atan(a);
    if (returnDegrees) {
        result = radiansToDegrees(result);
    }
    return result;
}

double Calculator::sineH(double a) {
    return std::sinh(a);
}

double Calculator::cosineH(double a) {
    return std::cosh(a);
}

double Calculator::tangentH(double a) {
    return std::tanh(a);
}

double Calculator::factorial(double a) {
    if (a < 0 || std::floor(a) != a) {
        throw std::invalid_argument("Factorial is defined only for non-negative integers");
    }
    
    if (a > 170) {
        throw std::invalid_argument("Factorial too large to compute");
    }
    
    if (a == 0 || a == 1) {
        return 1;
    }
    
    double result = 1;
    for (int i = 2; i <= (int)a; i++) {
        result *= i;
    }
    return result;
}

double Calculator::absolute(double a) {
    return std::abs(a);
}

double Calculator::percentage(double a) {
    return a / 100.0;
}

double Calculator::getPi() {
    return M_PI;
}

double Calculator::getE() {
    return M_E;
}

bool Calculator::isValidOperation(char operation) {
    return operation == '+' || operation == '-' || 
           operation == '*' || operation == '/' ||
           operation == '^' || operation == 's' ||
           operation == 'l' || operation == 'n' ||
           operation == '%';
}

double Calculator::calculate(double a, double b, char operation) {
    switch (operation) {
        case '+': return add(a, b);
        case '-': return subtract(a, b);
        case '*': return multiply(a, b);
        case '/': return divide(a, b);
        case '^': return power(a, b);
        case 's': return squareRoot(a);
        case 'l': return logarithm(a, b);
        case 'n': return naturalLogarithm(a);
        case '%': return percentage(a);
        default: throw std::invalid_argument("Invalid operation");
    }
}

double Calculator::calculateFunction(const std::string& funcName, double a) {
    auto it = functions.find(funcName);
    if (it != functions.end()) {
        try {
            return it->second(a);
        } catch (const std::exception& e) {
            throw std::runtime_error("Error calculating " + funcName + "(" + std::to_string(a) + "): " + e.what());
        }
    } else {
        throw std::invalid_argument("Unknown function: " + funcName);
    }
}
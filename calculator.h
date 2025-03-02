#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <map>
#include <functional>

class Calculator {
public:
    // Constructor
    Calculator();
    
    // Basic arithmetic operations
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);

    // Scientific operations
    double squareRoot(double a);
    double power(double base, double exponent);
    double logarithm(double a, double base);
    double naturalLogarithm(double a);
    
    // Trigonometric functions
    double sine(double a, bool inDegrees = true);
    double cosine(double a, bool inDegrees = true);
    double tangent(double a, bool inDegrees = true);
    
    // Inverse trigonometric functions
    double arcsine(double a, bool returnDegrees = true);
    double arccosine(double a, bool returnDegrees = true);
    double arctangent(double a, bool returnDegrees = true);
    
    // Hyperbolic functions
    double sineH(double a);
    double cosineH(double a);
    double tangentH(double a);
    
    // Constants
    double getPi();
    double getE();
    
    // Additional operations
    double factorial(double a);
    double absolute(double a);
    double percentage(double a);
    
    // Additional utility methods
    bool isValidOperation(char operation);
    double calculate(double a, double b, char operation);
    double calculateFunction(const std::string& funcName, double a);
    
    // Conversion between degrees and radians
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
    
private:
    std::map<std::string, std::function<double(double)>> functions;
    void initializeFunctions();
};

#endif // CALCULATOR_H 
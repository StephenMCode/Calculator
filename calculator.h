#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <map>
#include <functional>

class Calculator {
public:
    Calculator();
    
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);

    double squareRoot(double a);
    double power(double base, double exponent);
    double logarithm(double a, double base);
    double naturalLogarithm(double a);
    
    double sine(double a, bool inDegrees = true);
    double cosine(double a, bool inDegrees = true);
    double tangent(double a, bool inDegrees = true);
    
    double arcsine(double a, bool returnDegrees = true);
    double arccosine(double a, bool returnDegrees = true);
    double arctangent(double a, bool returnDegrees = true);
    
    double sineH(double a);
    double cosineH(double a);
    double tangentH(double a);
    
    double getPi();
    double getE();
    
    double factorial(double a);
    double absolute(double a);
    double percentage(double a);
    
    bool isValidOperation(char operation);
    double calculate(double a, double b, char operation);
    double calculateFunction(const std::string& funcName, double a);
    
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
    
private:
    std::map<std::string, std::function<double(double)>> functions;
    void initializeFunctions();
};

#endif
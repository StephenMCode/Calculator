#ifndef CALCULATOR_H
#define CALCULATOR_H

class Calculator {
public:
    // Constructor
    Calculator();
    
    // Basic arithmetic operations
    double add(double a, double b);
    double subtract(double a, double b);
    double multiply(double a, double b);
    double divide(double a, double b);
    
    // Additional utility methods
    bool isValidOperation(char operation);
    double calculate(double a, double b, char operation);
};

#endif // CALCULATOR_H 
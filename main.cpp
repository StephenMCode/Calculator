#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <codecvt>
#include <locale>
#include <stack>
#include <cctype>
#include <cmath>
#include <algorithm> // For std::min
#include "calculator.h"

// Add debug function for message boxes
void DebugMessage(const std::wstring& message) {
    MessageBoxW(NULL, message.c_str(), L"Debug Info", MB_OK | MB_ICONINFORMATION);
}

// Helper function to convert std::string to std::wstring
std::wstring StringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Helper function to convert std::wstring to std::string
std::string WStringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// Global variables
HWND hWndMain;                  // Main window handle
HWND hWndDisplay;               // Display field
HWND hWndMemoryIndicator;       // Memory indicator field
HWND hWndHistoryList;           // History list box
HWND hWndButtons[29];           // Button handles (increased for memory buttons)
Calculator calculator;          // Calculator instance
std::string currentExpression = "0";  // Current expression string
bool newExpression = true;      // Flag for new expression
double memoryValue = 0.0;       // Memory storage value
bool memoryHasValue = false;    // Flag indicating if memory has a value
std::vector<std::string> calculationHistory; // History of calculations

// Button definitions
struct ButtonDef {
    const wchar_t* label;
    int x;
    int y;
    int width;
    int height;
    COLORREF bgColor;
};

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateCalculatorUI(HWND hwnd);
void HandleButtonClick(const wchar_t* buttonText);
void UpdateDisplay(const std::string& text);
void UpdateMemoryIndicator();
void UpdateHistoryDisplay();
void CalculateExpression();
void ClearCalculator();
void ShowAboutDialog(HWND hwnd);
void MemoryAdd();
void MemorySubtract();
void MemoryRecall();
void MemoryClear();
bool IsOperator(char c);
int GetPrecedence(char op);
double ApplyOperator(double a, double b, char op);
double EvaluateExpression(const std::string& expression);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const wchar_t CLASS_NAME[] = L"CalculatorWindowClass";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClassW(&wc);
    
    // Create the window
    hWndMain = CreateWindowExW(
        0,                          // Optional window styles
        CLASS_NAME,                 // Window class
        L"C++ Calculator",           // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
        
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 550, // Increased width for history display
        
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    
    if (hWndMain == NULL) {
        return 0;
    }
    
    // Create calculator UI elements
    CreateCalculatorUI(hWndMain);
    
    // Initialize memory indicator
    UpdateMemoryIndicator();
    
    // Initialize history display
    UpdateHistoryDisplay();
    
    // Show the window
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            return 0;
            
        case WM_COMMAND: {
            int buttonId = LOWORD(wParam);
            
            // Handle button clicks
            if (buttonId >= 1000 && buttonId < 1029) {
                HWND buttonHwnd = (HWND)lParam;
                wchar_t buttonText[20];
                GetWindowTextW(buttonHwnd, buttonText, sizeof(buttonText)/sizeof(wchar_t));
                
                // Check if it's the About button
                if (wcscmp(buttonText, L"About") == 0) {
                    ShowAboutDialog(hwnd);
                } else {
                    HandleButtonClick(buttonText);
                }
            }
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// Create the calculator UI
void CreateCalculatorUI(HWND hwnd) {
    // Define button layout
    std::vector<ButtonDef> buttons = {
        // Row 1
        {L"C", 20, 100, 60, 50, RGB(255, 128, 128)},
        {L"s", 90, 100, 60, 50, RGB(173, 216, 230)},
        {L"^", 160, 100, 60, 50, RGB(173, 216, 230)},
        {L"/", 230, 100, 60, 50, RGB(173, 216, 230)},
        
        // Row 2
        {L"7", 20, 160, 60, 50, RGB(240, 240, 240)},
        {L"8", 90, 160, 60, 50, RGB(240, 240, 240)},
        {L"9", 160, 160, 60, 50, RGB(240, 240, 240)},
        {L"x", 230, 160, 60, 50, RGB(173, 216, 230)},
        
        // Row 3
        {L"4", 20, 220, 60, 50, RGB(240, 240, 240)},
        {L"5", 90, 220, 60, 50, RGB(240, 240, 240)},
        {L"6", 160, 220, 60, 50, RGB(240, 240, 240)},
        {L"-", 230, 220, 60, 50, RGB(173, 216, 230)},
        
        // Row 4
        {L"1", 20, 280, 60, 50, RGB(240, 240, 240)},
        {L"2", 90, 280, 60, 50, RGB(240, 240, 240)},
        {L"3", 160, 280, 60, 50, RGB(240, 240, 240)},
        {L"+", 230, 280, 60, 50, RGB(173, 216, 230)},
        
        // Row 5
        {L"0", 20, 340, 60, 50, RGB(240, 240, 240)},
        {L".", 90, 340, 60, 50, RGB(240, 240, 240)},
        {L"ln", 160, 340, 60, 50, RGB(173, 216, 230)},
        {L"=", 230, 340, 60, 50, RGB(144, 238, 144)},
        
        // Row 6 (new row for log and About)
        {L"log", 20, 400, 130, 40, RGB(173, 216, 230)},
        {L"About", 160, 400, 130, 40, RGB(200, 200, 200)},
        
        // Row 7 (memory buttons)
        {L"M+", 20, 450, 60, 50, RGB(255, 128, 128)},
        {L"M-", 90, 450, 60, 50, RGB(173, 216, 230)},
        {L"MR", 160, 450, 60, 50, RGB(240, 240, 240)},
        {L"MC", 230, 450, 60, 50, RGB(173, 216, 230)}
    };
    
    // Create memory indicator (small field to the right of the display)
    hWndMemoryIndicator = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        290, 20, 20, 25,
        hwnd, (HMENU)997, NULL, NULL
    );
    
    // Create main display field
    hWndDisplay = CreateWindowExW(
        0, L"EDIT", L"0",
        WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
        20, 20, 270, 50,
        hwnd, (HMENU)999, NULL, NULL
    );
    
    // Create history list box (to the right of the calculator)
    hWndHistoryList = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
        320, 20, 160, 480,
        hwnd, (HMENU)996, NULL, NULL
    );
    
    // Debug info about listbox creation
    std::wstring listboxDebugInfo = L"History display created with styles: " + 
                                   StringToWString(std::to_string(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL));
    DebugMessage(listboxDebugInfo);
    
    // Set a font for the history list box
    HFONT hHistoryFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hWndHistoryList, WM_SETFONT, (WPARAM)hHistoryFont, TRUE);
    
    // Set display fonts
    HFONT hFont = CreateFontW(28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hWndDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create buttons
    for (size_t i = 0; i < buttons.size(); i++) {
        const ButtonDef& btn = buttons[i];
        HWND hButton = CreateWindowExW(
            0, L"BUTTON", btn.label,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            btn.x, btn.y, btn.width, btn.height,
            hwnd, (HMENU)(1000 + i), NULL, NULL
        );
        
        // Set button font
        HFONT hBtnFont = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessage(hButton, WM_SETFONT, (WPARAM)hBtnFont, TRUE);
        
        // Store button handle
        hWndButtons[i] = hButton;
    }
}

// Handle button clicks
void HandleButtonClick(const wchar_t* buttonText) {
    // Convert wide string to narrow string for processing
    std::string narrowButtonText = WStringToString(buttonText);
    const char* buttonTextChar = narrowButtonText.c_str();
    
    // Debug for = button
    if (buttonTextChar[0] == '=') {
        std::wstring buttonDebugInfo = L"= button clicked. Current expression: " + 
                                      StringToWString(currentExpression);
        DebugMessage(buttonDebugInfo);
    }
    
    // Handle numeric buttons and decimal point
    if (isdigit(buttonTextChar[0]) || (buttonTextChar[0] == '.' && buttonTextChar[1] == '\0')) {
        if (newExpression) {
            currentExpression = buttonTextChar;
            newExpression = false;
        } else {
            // Don't allow multiple decimal points in a number
            if (buttonTextChar[0] == '.') {
                // Check if the last number already has a decimal point
                size_t lastOpPos = currentExpression.find_last_of("+-*/^");
                if (lastOpPos == std::string::npos) {
                    lastOpPos = 0;
                } else {
                    lastOpPos++; // Move past the operator
                }
                
                std::string lastNumber = currentExpression.substr(lastOpPos);
                if (lastNumber.find('.') != std::string::npos) {
                    return; // Already has a decimal point
                }
            }
            
            // If the current expression is just "0", replace it
            if (currentExpression == "0") {
                currentExpression = buttonTextChar;
            } else {
                currentExpression += buttonTextChar;
            }
        }
        UpdateDisplay(currentExpression);
        return;
    }
    
    // Handle memory operations
    if (wcscmp(buttonText, L"M+") == 0) {
        MemoryAdd();
        return;
    }
    
    if (wcscmp(buttonText, L"M-") == 0) {
        MemorySubtract();
        return;
    }
    
    if (wcscmp(buttonText, L"MR") == 0) {
        MemoryRecall();
        return;
    }
    
    if (wcscmp(buttonText, L"MC") == 0) {
        MemoryClear();
        return;
    }
    
    // Handle operations
    switch (buttonTextChar[0]) {
        case 'C': // Clear
            ClearCalculator();
            break;
            
        case '+': // Addition
        case '-': // Subtraction
            // Allow these operators at the beginning of an expression
            if (currentExpression == "0") {
                currentExpression = buttonTextChar;
            } else {
                // Check if the last character is an operator, replace it
                char lastChar = currentExpression.back();
                if (IsOperator(lastChar)) {
                    currentExpression.pop_back();
                }
                currentExpression += buttonTextChar;
            }
            newExpression = false;
            UpdateDisplay(currentExpression);
            break;
            
        case 'x': // Multiplication
            // Replace with * for calculation
            if (currentExpression != "0" && !IsOperator(currentExpression.back())) {
                currentExpression += "*";
                newExpression = false;
                UpdateDisplay(currentExpression);
            }
            break;
            
        case '/': // Division
        case '^': // Power
            // Don't allow these operators at the beginning of an expression
            if (currentExpression != "0" && !IsOperator(currentExpression.back())) {
                currentExpression += buttonTextChar;
                newExpression = false;
                UpdateDisplay(currentExpression);
            }
            break;
            
        case 's': // Square root
            try {
                // Calculate the square root of the current expression
                double expressionValue = EvaluateExpression(currentExpression);
                double result = calculator.squareRoot(expressionValue);
                
                // Format for history
                std::string historyEntry = "sqrt(" + currentExpression + ") = " + std::to_string(result);
                
                // Remove trailing zeros
                historyEntry.erase(historyEntry.find_last_not_of('0') + 1, std::string::npos);
                if (historyEntry.back() == '.') {
                    historyEntry.pop_back();
                }
                
                calculationHistory.push_back(historyEntry);
                
                // Limit history size to 20 entries
                if (calculationHistory.size() > 20) {
                    calculationHistory.erase(calculationHistory.begin());
                }
                
                // Update history display
                UpdateHistoryDisplay();
                
                // Set the result as the new expression
                currentExpression = std::to_string(result);
                // Remove trailing zeros
                currentExpression.erase(currentExpression.find_last_not_of('0') + 1, std::string::npos);
                if (currentExpression.back() == '.') {
                    currentExpression.pop_back();
                }
                
                UpdateDisplay(currentExpression);
                newExpression = true;
            } catch (const std::exception& e) {
                UpdateDisplay(e.what());
                newExpression = true;
            }
            break;
            
        case 'l': // Natural logarithm (ln) or logarithm (log)
            try {
                // Calculate the logarithm of the current expression
                double expressionValue = EvaluateExpression(currentExpression);
                double result;
                std::string funcName;
                
                if (wcscmp(buttonText, L"ln") == 0) {
                    result = calculator.naturalLogarithm(expressionValue);
                    funcName = "ln";
                } else if (wcscmp(buttonText, L"log") == 0) {
                    result = calculator.logarithm(expressionValue, 10.0); // Base 10 logarithm
                    funcName = "log";
                } else {
                    break;
                }
                
                // Format for history
                std::string historyEntry = funcName + "(" + currentExpression + ") = " + std::to_string(result);
                
                // Remove trailing zeros
                historyEntry.erase(historyEntry.find_last_not_of('0') + 1, std::string::npos);
                if (historyEntry.back() == '.') {
                    historyEntry.pop_back();
                }
                
                calculationHistory.push_back(historyEntry);
                
                // Limit history size to 20 entries
                if (calculationHistory.size() > 20) {
                    calculationHistory.erase(calculationHistory.begin());
                }
                
                // Update history display
                UpdateHistoryDisplay();
                
                // Set the result as the new expression
                currentExpression = std::to_string(result);
                // Remove trailing zeros
                currentExpression.erase(currentExpression.find_last_not_of('0') + 1, std::string::npos);
                if (currentExpression.back() == '.') {
                    currentExpression.pop_back();
                }
                
                UpdateDisplay(currentExpression);
                newExpression = true;
            } catch (const std::exception& e) {
                UpdateDisplay(e.what());
                newExpression = true;
            }
            break;
            
        case '=': // Calculate result
            DebugMessage(L"Calling CalculateExpression from = button");
            CalculateExpression();
            break;
    }
}

// Update the display
void UpdateDisplay(const std::string& text) {
    // Format the display text
    std::string displayText = text;
    
    // Replace "*" with "×" for display
    size_t pos = 0;
    while ((pos = displayText.find('*', pos)) != std::string::npos) {
        displayText.replace(pos, 1, "×");
        pos += 1; // Length of the replacement character
    }
    
    SetWindowTextW(hWndDisplay, StringToWString(displayText).c_str());
}

// Update the memory indicator
void UpdateMemoryIndicator() {
    if (memoryHasValue) {
        SetWindowTextW(hWndMemoryIndicator, L"M");
    } else {
        SetWindowTextW(hWndMemoryIndicator, L"");
    }
}

// Update the history display
void UpdateHistoryDisplay() {
    // Debug info about history size
    std::wstring historyDebugInfo = L"UpdateHistoryDisplay called. History size: " + 
                                   StringToWString(std::to_string(calculationHistory.size()));
    DebugMessage(historyDebugInfo);

    // Create the history text
    std::wstring historyText = L"Calculation History:\r\n";
    
    // Add each history item in reverse order (newest at the top)
    for (int i = calculationHistory.size() - 1; i >= 0; i--) {
        // Convert the history entry to a wide string
        std::wstring historyItem = StringToWString(calculationHistory[i]);
        
        // Debug info for each history item
        std::wstring itemDebugInfo = L"Adding history item #" + StringToWString(std::to_string(i)) + 
                                     L": [" + historyItem + L"] Length: " + 
                                     StringToWString(std::to_string(historyItem.length()));
        DebugMessage(itemDebugInfo);
        
        // Add the history item to the text
        historyText += historyItem + L"\r\n";
    }
    
    // Set the text to the history display
    SetWindowTextW(hWndHistoryList, historyText.c_str());
    
    // Debug final text
    std::wstring finalTextDebugInfo = L"Final history text: [" + historyText + L"]";
    DebugMessage(finalTextDebugInfo);
}

// Memory operations
void MemoryAdd() {
    try {
        double expressionValue = EvaluateExpression(currentExpression);
        memoryValue += expressionValue;
        memoryHasValue = true;
        UpdateMemoryIndicator();
        newExpression = true;
    } catch (...) {
        // Handle error
    }
}

void MemorySubtract() {
    try {
        double expressionValue = EvaluateExpression(currentExpression);
        memoryValue -= expressionValue;
        memoryHasValue = true;
        UpdateMemoryIndicator();
        newExpression = true;
    } catch (...) {
        // Handle error
    }
}

void MemoryRecall() {
    if (memoryHasValue) {
        if (newExpression) {
            currentExpression = std::to_string(memoryValue);
            // Remove trailing zeros
            currentExpression.erase(currentExpression.find_last_not_of('0') + 1, std::string::npos);
            if (currentExpression.back() == '.') {
                currentExpression.pop_back();
            }
            newExpression = false;
        } else {
            // If we're in the middle of an expression, only add the memory value if the last character is an operator
            if (IsOperator(currentExpression.back())) {
                std::string memStr = std::to_string(memoryValue);
                // Remove trailing zeros
                memStr.erase(memStr.find_last_not_of('0') + 1, std::string::npos);
                if (memStr.back() == '.') {
                    memStr.pop_back();
                }
                currentExpression += memStr;
            }
        }
        UpdateDisplay(currentExpression);
    }
}

void MemoryClear() {
    memoryValue = 0.0;
    memoryHasValue = false;
    UpdateMemoryIndicator();
}

// Calculate the expression
void CalculateExpression() {
    try {
        // Don't calculate if the expression ends with an operator
        if (IsOperator(currentExpression.back())) {
            return;
        }
        
        // Debug at start of calculation
        std::wstring startDebugInfo = L"CalculateExpression called with expression: " + 
                                     StringToWString(currentExpression);
        DebugMessage(startDebugInfo);
        
        // Store the original expression for history
        std::string originalExpression = currentExpression;
        
        // Evaluate the expression
        double result = EvaluateExpression(currentExpression);
        
        // Format the expression for display (replace * with ×)
        std::string displayExpression = originalExpression;
        size_t pos = 0;
        while ((pos = displayExpression.find('*', pos)) != std::string::npos) {
            displayExpression.replace(pos, 1, "×");
            pos += 1; // Length of the replacement character
        }
        
        // Format the result
        std::string resultStr = std::to_string(result);
        // Remove trailing zeros
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        if (resultStr.back() == '.') {
            resultStr.pop_back();
        }
        
        // Add to history
        std::string historyEntry = displayExpression + " = " + resultStr;
        
        // Debug history entry creation
        std::wstring historyDebugInfo = L"Created history entry: [" + 
                                       StringToWString(historyEntry) + L"] Length: " + 
                                       StringToWString(std::to_string(historyEntry.length()));
        DebugMessage(historyDebugInfo);
        
        calculationHistory.push_back(historyEntry);
        
        // Debug after adding to history vector
        std::wstring vectorDebugInfo = L"Added to history vector. New size: " + 
                                      StringToWString(std::to_string(calculationHistory.size()));
        DebugMessage(vectorDebugInfo);
        
        // Limit history size to 20 entries
        if (calculationHistory.size() > 20) {
            calculationHistory.erase(calculationHistory.begin());
        }
        
        // Update history display
        UpdateHistoryDisplay();
        
        // Set the result as the new expression
        currentExpression = resultStr;
        
        UpdateDisplay(currentExpression);
        newExpression = true;
    } catch (const std::exception& e) {
        // Debug exception
        std::wstring exceptionDebugInfo = L"Exception in CalculateExpression: " + 
                                         StringToWString(e.what());
        DebugMessage(exceptionDebugInfo);
        
        UpdateDisplay(e.what());
        ClearCalculator();
    }
}

// Clear calculator state
void ClearCalculator() {
    currentExpression = "0";
    newExpression = true;
    UpdateDisplay(currentExpression);
    // Note: Memory is not cleared by the C button
}

// Show About dialog
void ShowAboutDialog(HWND hwnd) {
    // Create a simple message box as an About dialog
    MessageBoxW(hwnd,
        L"C++ Calculator\n\n"
        L"Version 1.0\n\n"
        L"A comprehensive calculator application with both\n"
        L"basic arithmetic and scientific operations.\n\n"
        L"© 2024 - MIT License",
        L"About C++ Calculator",
        MB_OK | MB_ICONINFORMATION);
}

// Check if a character is an operator
bool IsOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Get operator precedence
int GetPrecedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 3;
    return 0;
}

// Apply an operator to two operands
double ApplyOperator(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) throw std::runtime_error("Division by zero");
            return a / b;
        case '^': return pow(a, b);
        default: return 0;
    }
}

// Evaluate a mathematical expression using the Shunting Yard algorithm
double EvaluateExpression(const std::string& expression) {
    std::stack<double> values;
    std::stack<char> operators;
    
    for (size_t i = 0; i < expression.length(); i++) {
        // Skip spaces
        if (expression[i] == ' ')
            continue;
        
        // If current character is an opening bracket, push it to operators stack
        if (expression[i] == '(') {
            operators.push(expression[i]);
        }
        // If current character is a closing bracket, solve the entire bracket
        else if (expression[i] == ')') {
            while (!operators.empty() && operators.top() != '(') {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = operators.top(); operators.pop();
                
                values.push(ApplyOperator(val1, val2, op));
            }
            
            // Remove the '(' from the stack
            if (!operators.empty())
                operators.pop();
        }
        // If current character is an operator
        else if (IsOperator(expression[i])) {
            // Handle unary minus
            if (expression[i] == '-' && (i == 0 || IsOperator(expression[i-1]) || expression[i-1] == '(')) {
                values.push(0);
            }
            
            // While top of operators has same or greater precedence
            while (!operators.empty() && GetPrecedence(operators.top()) >= GetPrecedence(expression[i])) {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = operators.top(); operators.pop();
                
                values.push(ApplyOperator(val1, val2, op));
            }
            
            // Push current operator to stack
            operators.push(expression[i]);
        }
        // If current character is a number
        else {
            std::string currentNumber;
            
            // Extract the complete number
            while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
                currentNumber += expression[i++];
            }
            i--; // Move back one position as the for loop will increment
            
            // Convert string to double and push to values stack
            values.push(std::stod(currentNumber));
        }
    }
    
    // Process all remaining operators
    while (!operators.empty()) {
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = operators.top(); operators.pop();
        
        values.push(ApplyOperator(val1, val2, op));
    }
    
    // Final result should be at the top of the values stack
    return values.top();
} 
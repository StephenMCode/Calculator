#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <codecvt>
#include <locale>
#include "calculator.h"

// Helper function to convert std::string to std::wstring
std::wstring StringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

// Global variables
HWND hWndMain;                  // Main window handle
HWND hWndDisplay;               // Display field
HWND hWndOperationDisplay;      // Operation display field
HWND hWndMemoryIndicator;       // Memory indicator field
HWND hWndButtons[29];           // Button handles (increased for memory buttons)
Calculator calculator;          // Calculator instance
std::string currentInput = "";  // Current input string
double firstNumber = 0.0;       // First operand
char currentOperation = '\0';   // Current operation
bool newCalculation = true;     // Flag for new calculation
bool waitingForSecondNumber = false; // Flag for waiting for second number
double memoryValue = 0.0;       // Memory storage value
bool memoryHasValue = false;    // Flag indicating if memory has a value

// Button definitions
struct ButtonDef {
    const char* label;
    int x;
    int y;
    int width;
    int height;
    COLORREF bgColor;
};

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateCalculatorUI(HWND hwnd);
void HandleButtonClick(const char* buttonText);
void UpdateDisplay(const std::string& text);
void UpdateOperationDisplay();
void UpdateMemoryIndicator();
void ProcessOperation(char operation);
void PerformCalculation();
void ClearCalculator();
void ShowAboutDialog(HWND hwnd);
void MemoryAdd();
void MemorySubtract();
void MemoryRecall();
void MemoryClear();

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char CLASS_NAME[] = "CalculatorWindowClass";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    
    // Create the window
    hWndMain = CreateWindowEx(
        0,                          // Optional window styles
        CLASS_NAME,                 // Window class
        "C++ Calculator",           // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
        
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 550, // Increased height for memory buttons
        
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
                char buttonText[20];
                GetWindowText(buttonHwnd, buttonText, sizeof(buttonText));
                
                // Check if it's the About button
                if (strcmp(buttonText, "About") == 0) {
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
        {"C", 20, 100, 60, 50, RGB(255, 128, 128)},
        {"s", 90, 100, 60, 50, RGB(173, 216, 230)},
        {"^", 160, 100, 60, 50, RGB(173, 216, 230)},
        {"/", 230, 100, 60, 50, RGB(173, 216, 230)},
        
        // Row 2
        {"7", 20, 160, 60, 50, RGB(240, 240, 240)},
        {"8", 90, 160, 60, 50, RGB(240, 240, 240)},
        {"9", 160, 160, 60, 50, RGB(240, 240, 240)},
        {"x", 230, 160, 60, 50, RGB(173, 216, 230)},
        
        // Row 3
        {"4", 20, 220, 60, 50, RGB(240, 240, 240)},
        {"5", 90, 220, 60, 50, RGB(240, 240, 240)},
        {"6", 160, 220, 60, 50, RGB(240, 240, 240)},
        {"-", 230, 220, 60, 50, RGB(173, 216, 230)},
        
        // Row 4
        {"1", 20, 280, 60, 50, RGB(240, 240, 240)},
        {"2", 90, 280, 60, 50, RGB(240, 240, 240)},
        {"3", 160, 280, 60, 50, RGB(240, 240, 240)},
        {"+", 230, 280, 60, 50, RGB(173, 216, 230)},
        
        // Row 5
        {"0", 20, 340, 60, 50, RGB(240, 240, 240)},
        {".", 90, 340, 60, 50, RGB(240, 240, 240)},
        {"ln", 160, 340, 60, 50, RGB(173, 216, 230)},
        {"=", 230, 340, 60, 50, RGB(144, 238, 144)},
        
        // Row 6 (new row for log and About)
        {"log", 20, 400, 130, 40, RGB(173, 216, 230)},
        {"About", 160, 400, 130, 40, RGB(200, 200, 200)},
        
        // Row 7 (memory buttons)
        {"M+", 20, 450, 60, 50, RGB(255, 128, 128)},
        {"M-", 90, 450, 60, 50, RGB(173, 216, 230)},
        {"MR", 160, 450, 60, 50, RGB(240, 240, 240)},
        {"MC", 230, 450, 60, 50, RGB(173, 216, 230)}
    };
    
    // Create operation display field (small field above the main display)
    hWndOperationDisplay = CreateWindowEx(
        0, "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
        20, 20, 270, 25,
        hwnd, (HMENU)998, NULL, NULL
    );
    
    // Create memory indicator (small field to the left of the operation display)
    hWndMemoryIndicator = CreateWindowEx(
        0, "STATIC", "",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        290, 20, 20, 25,
        hwnd, (HMENU)997, NULL, NULL
    );
    
    // Create main display field
    hWndDisplay = CreateWindowEx(
        0, "EDIT", "0",
        WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
        20, 50, 270, 40,
        hwnd, (HMENU)999, NULL, NULL
    );
    
    // Set display fonts
    HFONT hSmallFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    SendMessage(hWndOperationDisplay, WM_SETFONT, (WPARAM)hSmallFont, TRUE);
    
    HFONT hFont = CreateFont(28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    SendMessage(hWndDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    // Create buttons
    for (size_t i = 0; i < buttons.size(); i++) {
        const ButtonDef& btn = buttons[i];
        HWND hButton = CreateWindowEx(
            0, "BUTTON", btn.label,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            btn.x, btn.y, btn.width, btn.height,
            hwnd, (HMENU)(1000 + i), NULL, NULL
        );
        
        // Set button font
        HFONT hBtnFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        SendMessage(hButton, WM_SETFONT, (WPARAM)hBtnFont, TRUE);
        
        // Store button handle
        hWndButtons[i] = hButton;
    }
}

// Handle button clicks
void HandleButtonClick(const char* buttonText) {
    // Handle numeric buttons and decimal point
    if (isdigit(buttonText[0]) || (buttonText[0] == '.' && buttonText[1] == '\0')) {
        if (newCalculation || waitingForSecondNumber) {
            currentInput = buttonText;
            newCalculation = false;
            waitingForSecondNumber = false;
        } else {
            // Don't allow multiple decimal points
            if (buttonText[0] == '.' && currentInput.find('.') != std::string::npos) {
                return;
            }
            currentInput += buttonText;
        }
        UpdateDisplay(currentInput);
        return;
    }
    
    // Handle memory operations
    if (strcmp(buttonText, "M+") == 0) {
        MemoryAdd();
        return;
    }
    
    if (strcmp(buttonText, "M-") == 0) {
        MemorySubtract();
        return;
    }
    
    if (strcmp(buttonText, "MR") == 0) {
        MemoryRecall();
        return;
    }
    
    if (strcmp(buttonText, "MC") == 0) {
        MemoryClear();
        return;
    }
    
    // Handle operations
    if (strcmp(buttonText, "log") == 0) {
        // Logarithm with custom base
        if (!currentInput.empty()) {
            firstNumber = std::stod(currentInput);
            currentOperation = 'l';
            waitingForSecondNumber = true;
            newCalculation = false;
            UpdateOperationDisplay();
        }
        return;
    }
    
    switch (buttonText[0]) {
        case 'C': // Clear
            ClearCalculator();
            break;
            
        case '+': // Addition
        case '-': // Subtraction
        case 'x': // Multiplication
        case '/': // Division
        case '^': // Power
            if (!currentInput.empty()) {
                firstNumber = std::stod(currentInput);
            }
            
            // Map GUI symbols to calculator operations
            if (buttonText[0] == 'x') {
                currentOperation = '*';
            } else if (buttonText[0] == '/') {
                currentOperation = '/';
            } else if (buttonText[0] == '^') {
                currentOperation = 'p';
            } else {
                currentOperation = buttonText[0];
            }
            
            waitingForSecondNumber = true;
            newCalculation = false;
            UpdateOperationDisplay();
            break;
            
        case 's': // Square root
            if (!currentInput.empty()) {
                try {
                    double num = std::stod(currentInput);
                    double result = calculator.squareRoot(num);
                    currentInput = std::to_string(result);
                    // Remove trailing zeros
                    currentInput.erase(currentInput.find_last_not_of('0') + 1, std::string::npos);
                    if (currentInput.back() == '.') {
                        currentInput.pop_back();
                    }
                    UpdateDisplay(currentInput);
                    UpdateOperationDisplay(); // Clear operation display
                    newCalculation = true;
                } catch (const std::exception& e) {
                    UpdateDisplay(e.what());
                    UpdateOperationDisplay(); // Clear operation display
                    newCalculation = true;
                }
            }
            break;
            
        case 'l': // Natural logarithm (ln)
            if (strcmp(buttonText, "ln") == 0 && !currentInput.empty()) {
                try {
                    double num = std::stod(currentInput);
                    double result = calculator.naturalLogarithm(num);
                    currentInput = std::to_string(result);
                    // Remove trailing zeros
                    currentInput.erase(currentInput.find_last_not_of('0') + 1, std::string::npos);
                    if (currentInput.back() == '.') {
                        currentInput.pop_back();
                    }
                    UpdateDisplay(currentInput);
                    UpdateOperationDisplay(); // Clear operation display
                    newCalculation = true;
                } catch (const std::exception& e) {
                    UpdateDisplay(e.what());
                    UpdateOperationDisplay(); // Clear operation display
                    newCalculation = true;
                }
            }
            break;
            
        case '=': // Calculate result
            if (currentOperation != '\0' && !currentInput.empty()) {
                PerformCalculation();
            }
            break;
    }
}

// Update the display
void UpdateDisplay(const std::string& text) {
    // Format number for display (limit decimal places)
    if (text.find_first_not_of("0123456789.-") == std::string::npos) {
        try {
            double value = std::stod(text);
            std::ostringstream ss;
            ss.precision(10);
            ss << value;
            std::string formattedText = ss.str();
            
            // Remove trailing zeros after decimal point
            size_t decimalPos = formattedText.find('.');
            if (decimalPos != std::string::npos) {
                formattedText.erase(formattedText.find_last_not_of('0') + 1, std::string::npos);
                if (formattedText.back() == '.') {
                    formattedText.pop_back();
                }
            }
            
            SetWindowText(hWndDisplay, formattedText.c_str());
        } catch (...) {
            SetWindowText(hWndDisplay, text.c_str());
        }
    } else {
        // Display error message
        SetWindowText(hWndDisplay, text.c_str());
    }
}

// Update the operation display
void UpdateOperationDisplay() {
    std::string operationText = "";
    
    if (currentOperation != '\0') {
        std::ostringstream ss;
        ss << firstNumber;
        
        // Format the operation display
        switch (currentOperation) {
            case '+':
                operationText = ss.str() + " + ";
                break;
            case '-':
                operationText = ss.str() + " - ";
                break;
            case '*':
                operationText = ss.str() + " x ";
                break;
            case '/':
                operationText = ss.str() + " / ";
                break;
            case 'p':
                operationText = ss.str() + " ^ ";
                break;
            case 'l':
                operationText = "log_" + ss.str() + "(";
                break;
        }
    }
    
    SetWindowText(hWndOperationDisplay, operationText.c_str());
}

// Update the memory indicator
void UpdateMemoryIndicator() {
    if (memoryHasValue) {
        SetWindowText(hWndMemoryIndicator, "M");
    } else {
        SetWindowText(hWndMemoryIndicator, "");
    }
}

// Memory operations
void MemoryAdd() {
    if (!currentInput.empty()) {
        try {
            double value = std::stod(currentInput);
            memoryValue += value;
            memoryHasValue = true;
            UpdateMemoryIndicator();
            newCalculation = true;
        } catch (...) {
            // Handle error
        }
    }
}

void MemorySubtract() {
    if (!currentInput.empty()) {
        try {
            double value = std::stod(currentInput);
            memoryValue -= value;
            memoryHasValue = true;
            UpdateMemoryIndicator();
            newCalculation = true;
        } catch (...) {
            // Handle error
        }
    }
}

void MemoryRecall() {
    if (memoryHasValue) {
        currentInput = std::to_string(memoryValue);
        // Remove trailing zeros
        currentInput.erase(currentInput.find_last_not_of('0') + 1, std::string::npos);
        if (currentInput.back() == '.') {
            currentInput.pop_back();
        }
        UpdateDisplay(currentInput);
        newCalculation = false;
    }
}

void MemoryClear() {
    memoryValue = 0.0;
    memoryHasValue = false;
    UpdateMemoryIndicator();
}

// Perform calculation
void PerformCalculation() {
    try {
        double secondNumber = std::stod(currentInput);
        double result = calculator.calculate(firstNumber, secondNumber, currentOperation);
        
        // Convert result to string
        currentInput = std::to_string(result);
        // Remove trailing zeros
        currentInput.erase(currentInput.find_last_not_of('0') + 1, std::string::npos);
        if (currentInput.back() == '.') {
            currentInput.pop_back();
        }
        
        UpdateDisplay(currentInput);
        
        // Reset for next calculation
        firstNumber = result;
        currentOperation = '\0';
        newCalculation = true;
        UpdateOperationDisplay(); // Clear operation display
    } catch (const std::exception& e) {
        UpdateDisplay(e.what());
        ClearCalculator();
    }
}

// Clear calculator state
void ClearCalculator() {
    currentInput = "0";
    firstNumber = 0.0;
    currentOperation = '\0';
    newCalculation = true;
    waitingForSecondNumber = false;
    UpdateDisplay(currentInput);
    UpdateOperationDisplay(); // Clear operation display
    // Note: Memory is not cleared by the C button
}

// Show About dialog
void ShowAboutDialog(HWND hwnd) {
    // Create a simple message box as an About dialog
    MessageBox(hwnd,
        "C++ Calculator\n\n"
        "Version 1.0\n\n"
        "A comprehensive calculator application with both\n"
        "basic arithmetic and scientific operations.\n\n"
        "© 2024 - MIT License",
        "About C++ Calculator",
        MB_OK | MB_ICONINFORMATION);
} 
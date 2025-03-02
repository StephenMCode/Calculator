#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
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

// Add missing control messages
#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR (WM_USER + 67)
#endif

// Theme definitions
struct ThemeColors {
    std::wstring name;
    COLORREF windowBackground;
    COLORREF displayBackground;
    COLORREF displayText;
    COLORREF numericButtonBackground;
    COLORREF operatorButtonBackground;
    COLORREF equalButtonBackground;
    COLORREF clearButtonBackground;
    COLORREF memoryButtonBackground;
    COLORREF specialButtonBackground;
    COLORREF historyBackground;
    COLORREF historyText;
    // Replace the single buttonText with specific text colors for different button types
    COLORREF numericButtonText;
    COLORREF operatorButtonText;
    COLORREF equalButtonText;
    COLORREF clearButtonText;
    COLORREF memoryButtonText;
    COLORREF specialButtonText;
    COLORREF utilityButtonText; // For Theme and About buttons
};

// Theme collection
std::vector<ThemeColors> availableThemes = {
    // Default theme
    {
        L"Default",
        RGB(240, 240, 240),      // Window background
        RGB(255, 255, 255),      // Display background
        RGB(0, 0, 0),            // Display text
        RGB(240, 240, 240),      // Numeric button background
        RGB(173, 216, 230),      // Operator button background
        RGB(144, 238, 144),      // Equal button background
        RGB(255, 128, 128),      // Clear button background
        RGB(255, 128, 128),      // Memory button background
        RGB(173, 216, 230),      // Special button background
        RGB(255, 255, 255),      // History background
        RGB(0, 0, 0),            // History text
        RGB(0, 0, 0),            // Numeric button text
        RGB(0, 0, 0),            // Operator button text
        RGB(0, 60, 0),           // Equal button text (darker green for contrast)
        RGB(60, 0, 0),           // Clear button text (darker red for contrast)
        RGB(60, 0, 0),           // Memory button text
        RGB(0, 0, 80),           // Special button text (darker blue for contrast)
        RGB(0, 0, 0)             // Utility button text
    },
    // Dark theme
    {
        L"Dark",
        RGB(50, 50, 50),         // Window background
        RGB(30, 30, 30),         // Display background
        RGB(220, 220, 220),      // Display text
        RGB(70, 70, 70),         // Numeric button background
        RGB(100, 100, 140),      // Operator button background (slightly brighter blue)
        RGB(70, 140, 70),        // Equal button background (slightly brighter green)
        RGB(140, 70, 70),        // Clear button background (slightly brighter red)
        RGB(140, 70, 70),        // Memory button background
        RGB(70, 110, 140),       // Special button background (brighter for contrast)
        RGB(40, 40, 40),         // History background
        RGB(200, 200, 200),      // History text
        RGB(220, 220, 220),      // Numeric button text
        RGB(240, 240, 240),      // Operator button text (brighter for contrast)
        RGB(230, 255, 230),      // Equal button text (lighter green for contrast)
        RGB(255, 230, 230),      // Clear button text (lighter red for contrast) 
        RGB(255, 230, 230),      // Memory button text
        RGB(220, 235, 255),      // Special button text (lighter blue for contrast)
        RGB(220, 220, 220)       // Utility button text
    },
    // Blue theme
    {
        L"Blue",
        RGB(230, 240, 250),      // Window background
        RGB(240, 250, 255),      // Display background
        RGB(0, 30, 60),          // Display text
        RGB(200, 220, 240),      // Numeric button background
        RGB(150, 180, 210),      // Operator button background
        RGB(120, 200, 170),      // Equal button background
        RGB(240, 150, 150),      // Clear button background
        RGB(180, 180, 220),      // Memory button background
        RGB(150, 180, 210),      // Special button background
        RGB(240, 250, 255),      // History background
        RGB(0, 30, 60),          // History text
        RGB(0, 30, 60),          // Numeric button text
        RGB(0, 30, 80),          // Operator button text (darker blue for contrast)
        RGB(0, 60, 40),          // Equal button text (darker green for contrast)
        RGB(100, 0, 0),          // Clear button text (darker red for contrast)
        RGB(40, 40, 100),        // Memory button text (darker purple for contrast)
        RGB(0, 40, 80),          // Special button text (darker blue for contrast)
        RGB(30, 30, 30)          // Utility button text (dark gray for contrast)
    }
};

// Current theme index
int currentThemeIndex = 0;
ThemeColors* currentTheme = &availableThemes[currentThemeIndex];

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
void SwitchTheme();
void ApplyTheme(HWND hwnd);

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
    wc.style = CS_HREDRAW | CS_VREDRAW;  // Add style for better keyboard input handling
    
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
            
        case WM_KEYDOWN: {
            // Handle keyboard shortcuts
            int keyCode = static_cast<int>(wParam);
            bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            
            // Debug message to check key codes
            wchar_t debugMsg[100];
            swprintf(debugMsg, 100, L"Key pressed: %d, Shift: %d, Ctrl: %d", keyCode, shiftPressed, ctrlPressed);
            OutputDebugStringW(debugMsg);
            
            // Handle numeric keys (0-9)
            if ((keyCode >= '0' && keyCode <= '9') || 
                (keyCode >= VK_NUMPAD0 && keyCode <= VK_NUMPAD9)) {
                // Only handle numeric keys directly if shift is not pressed
                // This allows Shift+6 and Shift+8 to be handled by WM_CHAR
                if (!shiftPressed || (keyCode != '6' && keyCode != '8')) {
                    wchar_t keyChar;
                    if (keyCode >= '0' && keyCode <= '9') {
                        keyChar = static_cast<wchar_t>(keyCode);
                    } else {
                        // Convert numpad key to character
                        keyChar = static_cast<wchar_t>('0' + (keyCode - VK_NUMPAD0));
                    }
                    wchar_t buttonText[2] = { keyChar, L'\0' };
                    HandleButtonClick(buttonText);
                    return 0;
                }
            }
            
            // Handle decimal point
            if (keyCode == VK_DECIMAL || (keyCode == VK_OEM_PERIOD && !shiftPressed)) {
                HandleButtonClick(L".");
                return 0;
            }
            
            // Handle basic operations using a more consistent approach
            
            // Check for equals key (with and without shift)
            if (keyCode == VK_OEM_PLUS || keyCode == '=') {
                if (shiftPressed) {
                    // Shift+= is addition
                    HandleButtonClick(L"+");
                } else {
                    // = without shift is equals
                    HandleButtonClick(L"=");
                }
                return 0;
            }
            
            // Check for numpad addition
            if (keyCode == VK_ADD) {
                HandleButtonClick(L"+");
                return 0;
            }
            
            // Check for subtraction
            if (keyCode == VK_SUBTRACT || keyCode == VK_OEM_MINUS) {
                HandleButtonClick(L"-");
                return 0;
            }
            
            // Check for 8 key (with and without shift)
            if (keyCode == '8') {
                if (shiftPressed) {
                    // Shift+8 is multiplication
                    HandleButtonClick(L"x");
                } else {
                    // 8 without shift is just 8
                    HandleButtonClick(L"8");
                }
                return 0;
            }
            
            // Check for numpad multiplication and X key
            if (keyCode == VK_MULTIPLY || keyCode == 'X' || keyCode == 'x') {
                HandleButtonClick(L"x");
                return 0;
            }
            
            // Check for division
            if (keyCode == VK_DIVIDE || keyCode == VK_OEM_2 || 
                (keyCode == '/' && !shiftPressed)) {
                HandleButtonClick(L"/");
                return 0;
            }
            
            // Check for 6 key (with and without shift)
            if (keyCode == '6') {
                if (shiftPressed) {
                    // Shift+6 is power (^)
                    HandleButtonClick(L"^");
                } else {
                    // 6 without shift is just 6
                    HandleButtonClick(L"6");
                }
                return 0;
            }
            
            // Check for caret key directly
            if (keyCode == '^') {
                HandleButtonClick(L"^");
                return 0;
            }
            
            // Handle Enter key for calculation
            if (keyCode == VK_RETURN) {
                HandleButtonClick(L"=");
                return 0;
            }
            
            // Handle clear (ESC)
            if (keyCode == VK_ESCAPE) {
                HandleButtonClick(L"C");
                return 0;
            }
            
            // Handle special operations
            if (keyCode == 'S' || keyCode == 's') {  // Square root
                HandleButtonClick(L"s");
                return 0;
            }
            
            if (keyCode == 'L' || keyCode == 'l') {  // Natural logarithm
                if (shiftPressed) {
                    HandleButtonClick(L"log");  // Base 10 logarithm
                } else {
                    HandleButtonClick(L"ln");   // Natural logarithm
                }
                return 0;
            }
            
            // Handle memory operations
            if (keyCode == 'M' || keyCode == 'm') {
                if (shiftPressed && ctrlPressed) {
                    HandleButtonClick(L"MC");   // Memory Clear
                } else if (shiftPressed) {
                    HandleButtonClick(L"M-");   // Memory Subtract
                } else if (ctrlPressed) {
                    HandleButtonClick(L"MR");   // Memory Recall
                } else {
                    HandleButtonClick(L"M+");   // Memory Add
                }
                return 0;
            }
            
            return 0;
        }
        
        case WM_CHAR: {
            // Handle character input for special symbols
            int charCode = static_cast<int>(wParam);
            
            // Debug message to check character codes
            wchar_t debugMsg[100];
            swprintf(debugMsg, 100, L"Char received: %d", charCode);
            OutputDebugStringW(debugMsg);
            
            // Handle special characters from Shift+number combinations
            if (charCode == '^') {  // Shift+6 (power)
                HandleButtonClick(L"^");
                return 0;
            }
            else if (charCode == '*') {  // Shift+8 (multiplication)
                HandleButtonClick(L"x");
                return 0;
            }
            
            return 0;
        }
        
        case WM_COMMAND: {
            int buttonId = LOWORD(wParam);
            
            // Handle button clicks
            if (buttonId >= 1000 && buttonId < 1029) {
                HWND buttonHwnd = (HWND)lParam;
                wchar_t buttonText[20];
                GetWindowTextW(buttonHwnd, buttonText, sizeof(buttonText)/sizeof(wchar_t));
                
                // Check if it's a special button
                if (wcscmp(buttonText, L"About") == 0) {
                    ShowAboutDialog(hwnd);
                } else if (wcscmp(buttonText, L"Theme") == 0) {
                    SwitchTheme();
                    ApplyTheme(hwnd);
                } else {
                    HandleButtonClick(buttonText);
                }
            }
            return 0;
        }
        
        case WM_CTLCOLORBTN: {
            HDC hdcBtn = (HDC)wParam;
            HWND hwndBtn = (HWND)lParam;
            
            // Find which button this is
            int buttonIndex = -1;
            for (int i = 0; i < 29; i++) {
                if (hwndBtn == hWndButtons[i]) {
                    buttonIndex = i;
                    break;
                }
            }
            
            if (buttonIndex >= 0) {
                // Get corresponding button definition from buttons array
                // Since we can't access the buttons vector here, use a similar approach
                COLORREF bgColor = currentTheme->numericButtonBackground; // Default
                COLORREF txtColor = currentTheme->numericButtonText; // Default
                
                // Determine button type from its text
                wchar_t buttonText[20];
                GetWindowTextW(hwndBtn, buttonText, sizeof(buttonText)/sizeof(wchar_t));
                
                if (wcscmp(buttonText, L"C") == 0) {
                    bgColor = currentTheme->clearButtonBackground;
                    txtColor = currentTheme->clearButtonText;
                } else if (wcscmp(buttonText, L"=") == 0) {
                    bgColor = currentTheme->equalButtonBackground;
                    txtColor = currentTheme->equalButtonText;
                } else if (wcscmp(buttonText, L"+") == 0 || wcscmp(buttonText, L"-") == 0 ||
                           wcscmp(buttonText, L"x") == 0 || wcscmp(buttonText, L"/") == 0) {
                    bgColor = currentTheme->operatorButtonBackground;
                    txtColor = currentTheme->operatorButtonText;
                } else if (wcscmp(buttonText, L"M+") == 0 || wcscmp(buttonText, L"M-") == 0 ||
                           wcscmp(buttonText, L"MC") == 0 || wcscmp(buttonText, L"MR") == 0) {
                    bgColor = currentTheme->memoryButtonBackground;
                    txtColor = currentTheme->memoryButtonText;
                } else if (wcscmp(buttonText, L"s") == 0 || wcscmp(buttonText, L"^") == 0 ||
                          wcscmp(buttonText, L"ln") == 0 || wcscmp(buttonText, L"log") == 0) {
                    bgColor = currentTheme->specialButtonBackground;
                    txtColor = currentTheme->specialButtonText;
                } else if (wcscmp(buttonText, L"Theme") == 0 || wcscmp(buttonText, L"About") == 0) {
                    // For Theme and About buttons, use utility button colors
                    if (wcscmp(buttonText, L"Theme") == 0) {
                        bgColor = RGB(180, 180, 180);
                    } else {
                        bgColor = RGB(200, 200, 200);
                    }
                    txtColor = currentTheme->utilityButtonText;
                }
                
                SetBkColor(hdcBtn, bgColor);
                SetTextColor(hdcBtn, txtColor);
                return (LRESULT)CreateSolidBrush(bgColor);
            }
            break;
        }
        
        case WM_CTLCOLOREDIT: {
            HDC hdcEdit = (HDC)wParam;
            HWND hwndEdit = (HWND)lParam;
            
            if (hwndEdit == hWndDisplay) {
                SetTextColor(hdcEdit, currentTheme->displayText);
                SetBkColor(hdcEdit, currentTheme->displayBackground);
                return (LRESULT)CreateSolidBrush(currentTheme->displayBackground);
            } else if (hwndEdit == hWndHistoryList) {
                SetTextColor(hdcEdit, currentTheme->historyText);
                SetBkColor(hdcEdit, currentTheme->historyBackground);
                return (LRESULT)CreateSolidBrush(currentTheme->historyBackground);
            }
            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Create the calculator UI
void CreateCalculatorUI(HWND hwnd) {
    // Define button layout using current theme colors
    std::vector<ButtonDef> buttons = {
        // Row 1
        {L"C", 20, 100, 60, 50, currentTheme->clearButtonBackground},
        {L"s", 90, 100, 60, 50, currentTheme->specialButtonBackground},
        {L"^", 160, 100, 60, 50, currentTheme->specialButtonBackground},
        {L"/", 230, 100, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 2
        {L"7", 20, 160, 60, 50, currentTheme->numericButtonBackground},
        {L"8", 90, 160, 60, 50, currentTheme->numericButtonBackground},
        {L"9", 160, 160, 60, 50, currentTheme->numericButtonBackground},
        {L"x", 230, 160, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 3
        {L"4", 20, 220, 60, 50, currentTheme->numericButtonBackground},
        {L"5", 90, 220, 60, 50, currentTheme->numericButtonBackground},
        {L"6", 160, 220, 60, 50, currentTheme->numericButtonBackground},
        {L"-", 230, 220, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 4
        {L"1", 20, 280, 60, 50, currentTheme->numericButtonBackground},
        {L"2", 90, 280, 60, 50, currentTheme->numericButtonBackground},
        {L"3", 160, 280, 60, 50, currentTheme->numericButtonBackground},
        {L"+", 230, 280, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 5
        {L"0", 20, 340, 60, 50, currentTheme->numericButtonBackground},
        {L".", 90, 340, 60, 50, currentTheme->numericButtonBackground},
        {L"ln", 160, 340, 60, 50, currentTheme->specialButtonBackground},
        {L"=", 230, 340, 60, 50, currentTheme->equalButtonBackground},
        
        // Row 6 (new row for log, About, and Theme)
        {L"log", 20, 400, 90, 40, currentTheme->specialButtonBackground},
        {L"About", 120, 400, 80, 40, RGB(200, 200, 200)},
        {L"Theme", 210, 400, 80, 40, RGB(180, 180, 180)},
        
        // Row 7 (memory buttons)
        {L"M+", 20, 450, 60, 50, currentTheme->memoryButtonBackground},
        {L"M-", 90, 450, 60, 50, currentTheme->memoryButtonBackground},
        {L"MR", 160, 450, 60, 50, currentTheme->numericButtonBackground},
        {L"MC", 230, 450, 60, 50, currentTheme->memoryButtonBackground}
    };
    
    // Set window background color
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(currentTheme->windowBackground));
    
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
    
    // Set display colors
    SendMessage(hWndDisplay, EM_SETBKGNDCOLOR, 0, currentTheme->displayBackground);
    SendMessage(hWndDisplay, WM_CTLCOLOREDIT, 0, (LPARAM)currentTheme->displayText);
    
    // Create history list box (to the right of the calculator)
    hWndHistoryList = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
        320, 20, 160, 480,
        hwnd, (HMENU)996, NULL, NULL
    );
    
    // Set history colors
    SendMessage(hWndHistoryList, EM_SETBKGNDCOLOR, 0, currentTheme->historyBackground);
    
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
        
        // Store button handle
        hWndButtons[i] = hButton;
        
        // Set button font
        HFONT hBtnFont = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessage(hButton, WM_SETFONT, (WPARAM)hBtnFont, TRUE);
        
        // Set button color
        // This is done using owner-drawn buttons with WM_CTLCOLORBTN
        SendMessage(hButton, WM_CTLCOLORBTN, 0, (LPARAM)btn.bgColor);
    }
}

// Handle button clicks
void HandleButtonClick(const wchar_t* buttonText) {
    // Convert wide string to narrow string for processing
    std::string narrowButtonText = WStringToString(buttonText);
    const char* buttonTextChar = narrowButtonText.c_str();
    
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
    // Create the history text
    std::wstring historyText = L"Calculation History:\r\n";
    
    // Add each history item in reverse order (newest at the top)
    for (int i = calculationHistory.size() - 1; i >= 0; i--) {
        // Convert the history entry to a wide string
        std::wstring historyItem = StringToWString(calculationHistory[i]);
        
        // Add the history item to the text
        historyText += historyItem + L"\r\n";
    }
    
    // Set the text to the history display
    SetWindowTextW(hWndHistoryList, historyText.c_str());
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
        
        calculationHistory.push_back(historyEntry);
        
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
        L"Version 1.1\n\n"
        L"A comprehensive calculator application with both\n"
        L"basic arithmetic and scientific operations.\n\n"
        L"Features:\n"
        L"- Basic & scientific operations\n"
        L"- Memory functions\n"
        L"- Complex expression evaluation\n"
        L"- Calculation history\n"
        L"- Customizable themes\n"
        L"- Keyboard shortcuts\n\n"
        L"Keyboard Shortcuts:\n"
        L"- 0-9: Number keys (numpad also works)\n"
        L"- +, -, *, /: Basic operations\n"
        L"- Enter: Calculate (=)\n"
        L"- Esc: Clear (C)\n"
        L"- . (period): Decimal point\n"
        L"- S: Square root\n"
        L"- ^ (Shift+6): Power\n"
        L"- L: Natural logarithm (ln)\n"
        L"- Shift+L: Base-10 logarithm (log)\n"
        L"- M: Memory add (M+)\n"
        L"- Shift+M: Memory subtract (M-)\n"
        L"- Ctrl+M: Memory recall (MR)\n"
        L"- Ctrl+Shift+M: Memory clear (MC)\n\n"
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

// Function to switch to the next theme
void SwitchTheme() {
    // Cycle to the next theme
    currentThemeIndex = (currentThemeIndex + 1) % availableThemes.size();
    currentTheme = &availableThemes[currentThemeIndex];
    
    // Show theme change message
    std::wstring message = L"Theme changed to: " + currentTheme->name;
    MessageBoxW(NULL, message.c_str(), L"Theme Changed", MB_OK | MB_ICONINFORMATION);
}

// Function to apply the current theme to all UI elements
void ApplyTheme(HWND hwnd) {
    // Recreate the UI with the new theme
    // First, destroy all child windows
    EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
        DestroyWindow(hwndChild);
        return TRUE;
    }, 0);
    
    // Recreate the UI
    CreateCalculatorUI(hwnd);
    
    // Refresh the memory indicator and history
    UpdateMemoryIndicator();
    UpdateHistoryDisplay();
    
    // Force repaint
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
} 
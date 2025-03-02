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
#include <fstream>
#include <iomanip>  // For std::setprecision, etc.
#include <ctime>    // For time logging

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

// Global debug log file stream
std::ofstream debugLogFile;

// Debug logging functions
void initDebugLog() {
    // Clear and open the log file
    debugLogFile.open("calculator_debug.log", std::ios::out | std::ios::trunc);
    
    // Log header with timestamp
    time_t now = time(0);
    char timeStr[26];
    ctime_s(timeStr, sizeof(timeStr), &now);
    
    debugLogFile << "------------------------------------------------------------------------------" << std::endl;
    debugLogFile << "Calculator Debug Log - Started: " << timeStr;
    debugLogFile << "------------------------------------------------------------------------------" << std::endl;
    debugLogFile << "Build version: " << __DATE__ << " " << __TIME__ << std::endl;
    debugLogFile << std::endl;
    
    // Configure stream formatting
    debugLogFile << std::fixed << std::setprecision(6);
    
    // Flush to ensure header is written immediately
    debugLogFile.flush();
}

void closeDebugLog() {
    if (debugLogFile.is_open()) {
        // Log footer with timestamp
        time_t now = time(0);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now);
        
        debugLogFile << std::endl;
        debugLogFile << "------------------------------------------------------------------------------" << std::endl;
        debugLogFile << "Calculator Debug Log - Ended: " << timeStr;
        debugLogFile << "------------------------------------------------------------------------------" << std::endl;
        
        // Close the file
        debugLogFile.close();
    }
}

// Helper function to log with timestamp
void logDebug(const std::string& message, const std::string& category = "INFO") {
    if (debugLogFile.is_open()) {
        // Get current time
        SYSTEMTIME st;
        GetLocalTime(&st);
        
        // Format: [HH:MM:SS.mmm] [CATEGORY] Message
        debugLogFile << "[" 
                   << std::setw(2) << std::setfill('0') << st.wHour << ":"
                   << std::setw(2) << std::setfill('0') << st.wMinute << ":" 
                   << std::setw(2) << std::setfill('0') << st.wSecond << "." 
                   << std::setw(3) << std::setfill('0') << st.wMilliseconds << "] "
                   << "[" << category << "] " << message << std::endl;
        
        // Flush to ensure message is written immediately
        debugLogFile.flush();
    }
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
HWND hWndButtons[45];           // Button handles (increased for additional function buttons)
Calculator calculator;          // Calculator instance
std::string currentExpression = "0";  // Current expression string
std::string previousExpression = "0"; // For undo functionality
bool newExpression = true;      // Flag for new expression
double memoryValue = 0.0;       // Memory storage value
bool memoryUsed = false;        // Flag indicating if memory has a value
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
std::string FormatExpressionWithPrecedence(const std::string& expr);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize debug log
    initDebugLog();
    logDebug("Application started", "MAIN");
    
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
    
    // Calculate screen dimensions for centering
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Window dimensions
    int windowWidth = 520;   // Increased width to accommodate history display better
    int windowHeight = 680;  // Increased height to accommodate all buttons
    
    // Calculate window position to center on screen
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;
    
    // Create the window
    hWndMain = CreateWindowExW(
        0,                          // Optional window styles
        CLASS_NAME,                 // Window class
        L"Advanced C++ Calculator",  // Updated window title
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
        
        // Size and position (centered on screen)
        windowX, windowY, windowWidth, windowHeight,
        
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
    
    // Before returning, close the debug log
    logDebug("Application ending", "MAIN");
    closeDebugLog();
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
            
            // Check for percent key
            if (keyCode == '%' || (keyCode == '5' && shiftPressed)) {
                HandleButtonClick(L"%");
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
            
            // Handle backspace for delete
            if (keyCode == VK_BACK) {
                HandleButtonClick(L"DEL");
                return 0;
            }
            
            // Handle special operations
            if (keyCode == 'S' || keyCode == 's') {  // Square root
                HandleButtonClick(L"sqrt");
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
            
            // Log button clicks for debugging
            std::string debugMsg = "WM_COMMAND received, buttonId: " + std::to_string(buttonId);
            logDebug(debugMsg, "UI");
            
            // Handle button clicks
            if (buttonId >= 1000 && buttonId <= 1050) {
                // Get the button text
                HWND buttonHwnd = (HWND)lParam;
                wchar_t buttonText[50];
                GetWindowTextW(buttonHwnd, buttonText, 50);
                
                std::string narrowBtnText = WStringToString(buttonText);
                std::string btnClickMsg = "Button clicked in WindowProc: " + narrowBtnText;
                logDebug(btnClickMsg, "UI");
                
                // Special handling for theme button
                if (wcscmp(buttonText, L"THEME") == 0) {
                    logDebug("Theme button detected in WindowProc", "UI");
                    SwitchTheme();
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    return 0;
                }
                
                // Special handling for UNDO button
                if (wcscmp(buttonText, L"UNDO") == 0) {
                    logDebug("UNDO button detected in WindowProc", "UI");
                    HandleButtonClick(buttonText);
                    return 0;
                }
                
                // Regular button handling
                logDebug("Regular button detected in WindowProc", "UI");
                HandleButtonClick(buttonText);
            }
            break;
        }
        
        case WM_CTLCOLORBTN: {
            HDC hdcBtn = (HDC)wParam;
            HWND hwndBtn = (HWND)lParam;
            
            // Find which button this is
            int buttonIndex = -1;
            for (int i = 0; i < 45; i++) {
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
            logDebug("WM_DESTROY received, closing application", "MAIN");
            closeDebugLog();
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
        // Row 1 (Function row 1)
        {L"sin", 20, 100, 50, 40, currentTheme->specialButtonBackground},
        {L"cos", 80, 100, 50, 40, currentTheme->specialButtonBackground},
        {L"tan", 140, 100, 50, 40, currentTheme->specialButtonBackground},
        {L"pi", 200, 100, 40, 40, currentTheme->specialButtonBackground},
        {L"e", 250, 100, 40, 40, currentTheme->specialButtonBackground},
        
        // Row 2 (Function row 2)
        {L"asin", 20, 150, 50, 40, currentTheme->specialButtonBackground},
        {L"acos", 80, 150, 50, 40, currentTheme->specialButtonBackground},
        {L"atan", 140, 150, 50, 40, currentTheme->specialButtonBackground},
        {L"(", 200, 150, 40, 40, currentTheme->operatorButtonBackground},
        {L")", 250, 150, 40, 40, currentTheme->operatorButtonBackground},
        
        // Row 3
        {L"C", 20, 200, 60, 50, currentTheme->clearButtonBackground},
        {L"sqrt", 90, 200, 60, 50, currentTheme->specialButtonBackground},
        {L"^", 160, 200, 60, 50, currentTheme->specialButtonBackground},
        {L"/", 230, 200, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 4
        {L"7", 20, 260, 60, 50, currentTheme->numericButtonBackground},
        {L"8", 90, 260, 60, 50, currentTheme->numericButtonBackground},
        {L"9", 160, 260, 60, 50, currentTheme->numericButtonBackground},
        {L"x", 230, 260, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 5
        {L"4", 20, 320, 60, 50, currentTheme->numericButtonBackground},
        {L"5", 90, 320, 60, 50, currentTheme->numericButtonBackground},
        {L"6", 160, 320, 60, 50, currentTheme->numericButtonBackground},
        {L"-", 230, 320, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 6
        {L"1", 20, 380, 60, 50, currentTheme->numericButtonBackground},
        {L"2", 90, 380, 60, 50, currentTheme->numericButtonBackground},
        {L"3", 160, 380, 60, 50, currentTheme->numericButtonBackground},
        {L"+", 230, 380, 60, 50, currentTheme->operatorButtonBackground},
        
        // Row 7
        {L"0", 20, 440, 60, 50, currentTheme->numericButtonBackground},
        {L".", 90, 440, 60, 50, currentTheme->numericButtonBackground},
        {L"%", 160, 440, 60, 50, currentTheme->operatorButtonBackground},
        {L"=", 230, 440, 60, 50, currentTheme->equalButtonBackground},
        
        // Row 8 (Memory and functions)
        {L"M+", 20, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"M-", 80, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"MR", 140, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"MC", 200, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"ln", 260, 500, 50, 40, currentTheme->specialButtonBackground},
        
        // Row 9 (Utility)
        {L"log", 20, 550, 60, 40, currentTheme->specialButtonBackground},
        {L"fact", 90, 550, 60, 40, currentTheme->specialButtonBackground},
        {L"abs", 160, 550, 60, 40, currentTheme->specialButtonBackground},
        {L"Theme", 230, 550, 80, 40, RGB(180, 180, 180)},
        
        // Row 10
        {L"About", 20, 600, 80, 40, RGB(200, 200, 200)},
        {L"DEL", 110, 600, 80, 40, currentTheme->clearButtonBackground},
        {L"UNDO", 200, 600, 80, 40, currentTheme->specialButtonBackground}
    };
    
    // Set window background color
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(currentTheme->windowBackground));
    
    // Create memory indicator (small field to the right of the display)
    hWndMemoryIndicator = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        300, 20, 20, 25,
        hwnd, (HMENU)997, NULL, NULL
    );
    
    // Create main display field
    hWndDisplay = CreateWindowExW(
        0, L"EDIT", L"0",
        WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
        20, 20, 280, 60,  // Increased height and width
        hwnd, (HMENU)999, NULL, NULL
    );
    
    // Set display colors
    SendMessage(hWndDisplay, EM_SETBKGNDCOLOR, 0, currentTheme->displayBackground);
    SendMessage(hWndDisplay, WM_CTLCOLOREDIT, 0, (LPARAM)currentTheme->displayText);
    
    // Create history list box (to the right of the calculator)
    hWndHistoryList = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
        320, 20, 160, 620,
        hwnd, (HMENU)996, NULL, NULL
    );
    
    // Set history colors
    SendMessage(hWndHistoryList, EM_SETBKGNDCOLOR, 0, currentTheme->historyBackground);
    
    // Set a font for the history list box
    HFONT hHistoryFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hWndHistoryList, WM_SETFONT, (WPARAM)hHistoryFont, TRUE);
    
    // Set display fonts - increased font size
    HFONT hFont = CreateFontW(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
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
    std::string narrowBtnText = WStringToString(buttonText);
    std::string debugMsg = "HandleButtonClick called with button: " + narrowBtnText;
    logDebug(debugMsg, "UI");
    
    // Special handling for equals button
    if (wcscmp(buttonText, L"=") == 0) {
        logDebug("Equals button detected, calling CalculateExpression()", "CALC");
        std::string debugExpr = "Current expression before calculation: " + currentExpression;
        logDebug(debugExpr, "CALC");
        CalculateExpression();
        return;
    }
    
    // Check for constants
    if (wcscmp(buttonText, L"pi") == 0) {
        if (newExpression) {
            currentExpression = "pi";
            newExpression = false;
        } else if (currentExpression == "0") {
            currentExpression = "pi";
        } else {
            // Handle adding pi after operators
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "pi";
            } else {
                // Otherwise we're multiplying
                currentExpression += "*pi";
            }
        }
        UpdateDisplay(currentExpression);
        return;
    }
    
    if (wcscmp(buttonText, L"e") == 0) {
        if (newExpression) {
            currentExpression = "e";
            newExpression = false;
        } else if (currentExpression == "0") {
            currentExpression = "e";
        } else {
            // Handle adding e after operators
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "e";
            } else {
                // Otherwise we're multiplying
                currentExpression += "*e";
            }
        }
        UpdateDisplay(currentExpression);
        return;
    }
    
    // Handle parentheses
    if (wcscmp(buttonText, L"(") == 0) {
        if (newExpression) {
            currentExpression = "(";
            newExpression = false;
        } else if (currentExpression == "0") {
            currentExpression = "(";
        } else {
            // Handle adding opening parenthesis after operators or another opening parenthesis
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "(";
            } else {
                // Otherwise we're multiplying
                currentExpression += "*(";
            }
        }
        UpdateDisplay(currentExpression);
        return;
    }
    
    if (wcscmp(buttonText, L")") == 0) {
        // Only add closing parenthesis if there's an opening one
        int openCount = 0, closeCount = 0;
        for (char c : currentExpression) {
            if (c == '(') openCount++;
            if (c == ')') closeCount++;
        }
        if (openCount > closeCount) {
            currentExpression += ")";
            UpdateDisplay(currentExpression);
        }
        return;
    }
    
    // Handle trigonometric and other functions
    if (wcscmp(buttonText, L"sin") == 0 || wcscmp(buttonText, L"cos") == 0 || 
        wcscmp(buttonText, L"tan") == 0 || wcscmp(buttonText, L"asin") == 0 || 
        wcscmp(buttonText, L"acos") == 0 || wcscmp(buttonText, L"atan") == 0 ||
        wcscmp(buttonText, L"sinh") == 0 || wcscmp(buttonText, L"cosh") == 0 || 
        wcscmp(buttonText, L"tanh") == 0 || wcscmp(buttonText, L"abs") == 0 ||
        wcscmp(buttonText, L"fact") == 0) {
        
        logDebug("Function button clicked: " + narrowBtnText, "UI");
        
        if (newExpression || currentExpression == "0") {
            currentExpression = narrowBtnText + "(";
            newExpression = false;
        } else {
            // Handle adding function after operators
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += narrowBtnText + "(";
            } else {
                // Otherwise we're multiplying
                currentExpression += "*" + narrowBtnText + "(";
            }
        }
        
        // Count unclosed parentheses for debugging
        int openCount = 0, closeCount = 0;
        for (char c : currentExpression) {
            if (c == '(') openCount++;
            if (c == ')') closeCount++;
        }
        if (openCount > closeCount) {
            logDebug("Detected " + std::to_string(openCount - closeCount) + " unclosed parentheses", "UI");
        }
        
        UpdateDisplay(currentExpression);
        return;
    }
    
    // Handle sqrt button
    if (wcscmp(buttonText, L"sqrt") == 0) {
        logDebug("Square root function clicked: sqrt", "UI");
        
        if (newExpression || currentExpression == "0") {
            currentExpression = "sqrt(";
            newExpression = false;
        } else {
            // Handle adding function after operators
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "sqrt(";
            } else {
                // Otherwise we're multiplying
                currentExpression += "*sqrt(";
            }
        }
        
        // Count unclosed parentheses for debugging
        int openCount = 0, closeCount = 0;
        for (char c : currentExpression) {
            if (c == '(') openCount++;
            if (c == ')') closeCount++;
        }
        if (openCount > closeCount) {
            logDebug("Detected " + std::to_string(openCount - closeCount) + " unclosed parentheses", "UI");
        }
        
        UpdateDisplay(currentExpression);
        return;
    }
    
    // Handle delete button
    if (wcscmp(buttonText, L"DEL") == 0) {
        if (currentExpression.length() > 0 && !newExpression && currentExpression != "0") {
            // Save the current expression for undo
            previousExpression = currentExpression;
            logDebug("Saved expression for undo: " + previousExpression, "UI");
            
            // Enhanced delete functionality to handle semantic units
            std::string debugMsg = "DEL button pressed, current expression: " + currentExpression;
            logDebug(debugMsg, "UI");
            
            // Check if we're deleting a function call
            std::vector<std::string> functions = {"sin", "cos", "tan", "asin", "acos", "atan", 
                                                "sinh", "cosh", "tanh", "sqrt", "log", "ln",
                                                "abs", "fact"};
            
            bool deletedFunction = false;
            for (const std::string& func : functions) {
                // Check if expression ends with function name and opening parenthesis
                if (currentExpression.length() >= func.length() + 1 && 
                    currentExpression.substr(currentExpression.length() - (func.length() + 1)) == func + "(") {
                    // Delete the entire function call
                    currentExpression.erase(currentExpression.length() - (func.length() + 1));
                    deletedFunction = true;
                    logDebug("Deleted function: " + func + "(", "UI");
                    break;
                }
            }
            
            // If no function was deleted, check for other semantic units
            if (!deletedFunction) {
                // Check for constants (pi, e)
                if (currentExpression.length() >= 2 && currentExpression.substr(currentExpression.length() - 2) == "pi") {
                    currentExpression.erase(currentExpression.length() - 2);
                    logDebug("Deleted constant: pi", "UI");
                } else if (currentExpression.length() >= 1 && currentExpression.back() == 'e') {
                    currentExpression.pop_back();
                    logDebug("Deleted constant: e", "UI");
                } else {
                    // Default behavior: delete one character
                    currentExpression.pop_back();
                    logDebug("Deleted single character", "UI");
                }
            }
            
            if (currentExpression.empty()) {
                currentExpression = "0";
            }
            UpdateDisplay(currentExpression);
        }
        return;
    }
    
    // Handle undo button (CTRL+Z)
    if (wcscmp(buttonText, L"UNDO") == 0 || wcscmp(buttonText, L"Ctrl+Z") == 0) {
        if (previousExpression != "0") {
            // Swap current and previous expressions
            std::string temp = currentExpression;
            currentExpression = previousExpression;
            previousExpression = temp;
            
            logDebug("Undo performed, restored: " + currentExpression, "UI");
            UpdateDisplay(currentExpression);
        }
        return;
    }
    
    // Handle numeric buttons and decimal point
    if (isdigit(narrowBtnText[0]) || (narrowBtnText[0] == '.' && narrowBtnText[1] == '\0')) {
        if (newExpression) {
            currentExpression = narrowBtnText;
            newExpression = false;
        } else {
            // Don't allow multiple decimal points in a number
            if (narrowBtnText[0] == '.') {
                // Check if the last number already has a decimal point
                size_t lastOpPos = currentExpression.find_last_of("+-*/^%()");
                if (lastOpPos == std::string::npos) {
                    lastOpPos = 0;
                } else {
                    lastOpPos++; // Move past the operator
                }
                
                std::string lastNumber = currentExpression.substr(lastOpPos);
                if (lastNumber.find('.') != std::string::npos) {
                    return; // Don't add another decimal point
                }
            }
            currentExpression += narrowBtnText;
        }
        UpdateDisplay(currentExpression);
    }
    // Handle operators (+, -, *, /, ^, %)
    else if (wcscmp(buttonText, L"+") == 0 || wcscmp(buttonText, L"-") == 0 || 
             wcscmp(buttonText, L"x") == 0 || wcscmp(buttonText, L"/") == 0 || 
             wcscmp(buttonText, L"^") == 0 || wcscmp(buttonText, L"%") == 0) {
        
        // Don't add operator if expression is empty or ends with an operator
        if (currentExpression.empty() || currentExpression == "0") {
            if (wcscmp(buttonText, L"-") == 0) {
                // Allow negative numbers
                currentExpression = "-";
                newExpression = false;
                UpdateDisplay(currentExpression);
            }
            return;
        }
        
        // Replace 'x' with '*' for internal representation
        std::string opStr = narrowBtnText;
        if (opStr == "x") {
            opStr = "*";
        }
        
        // If the last character is already an operator, replace it
        char lastChar = currentExpression.back();
        if (IsOperator(lastChar)) {
            currentExpression.pop_back();
            currentExpression += opStr;
        } else {
            currentExpression += opStr;
        }
        
        newExpression = false;
        UpdateDisplay(currentExpression);
    }
    else if (wcscmp(buttonText, L"ln") == 0 || wcscmp(buttonText, L"log") == 0) {
        // Natural logarithm or logarithm
        std::string funcName;
        if (wcscmp(buttonText, L"ln") == 0) {
            funcName = "ln";
        } else {
            funcName = "log";
        }
        
        logDebug("Logarithm function clicked: " + funcName, "UI");
        
        if (newExpression || currentExpression == "0") {
            currentExpression = funcName + "(";
            newExpression = false;
        } else {
            // Handle adding function after operators
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += funcName + "(";
            } else {
                // Otherwise we're multiplying
                currentExpression += "*" + funcName + "(";
            }
        }
        
        // Count unclosed parentheses for debugging
        int openCount = 0, closeCount = 0;
        for (char c : currentExpression) {
            if (c == '(') openCount++;
            if (c == ')') closeCount++;
        }
        if (openCount > closeCount) {
            logDebug("Detected " + std::to_string(openCount - closeCount) + " unclosed parentheses", "UI");
        }
        
        UpdateDisplay(currentExpression);
    }
    else if (wcscmp(buttonText, L"Theme") == 0) {
        // Theme button
        logDebug("Theme button clicked, switching theme", "UI");
        SwitchTheme();
        ApplyTheme(hWndMain);
    }
    else if (wcscmp(buttonText, L"About") == 0) {
        // About button
        logDebug("About button clicked, showing dialog", "UI");
        ShowAboutDialog(hWndMain);
    }
    else if (wcscmp(buttonText, L"C") == 0) {
        // Clear button
        ClearCalculator();
    }
}

// Update the display
void UpdateDisplay(const std::string& text) {
    // Format the display text with operator precedence visualization
    std::string displayText = FormatExpressionWithPrecedence(text);
    
    // Check for unbalanced parentheses and add visual indicator
    int openCount = 0, closeCount = 0;
    for (char c : displayText) {
        if (c == '(') openCount++;
        if (c == ')') closeCount++;
    }
    
    // If there are unclosed parentheses, add a visual indicator
    std::string parenthesesIndicator = "";
    if (openCount > closeCount) {
        int missingCount = openCount - closeCount;
        
        // Determine if we should show the indicator
        bool shouldShowIndicator = true;
        
        // Common functions that would add an opening parenthesis
        std::vector<std::string> functions = {"sin", "cos", "tan", "asin", "acos", "atan",
                                            "sinh", "cosh", "tanh", "sqrt", "log", "ln",
                                            "abs", "fact"};
        
        // Check if we're inside a single function call without nested parentheses
        if (missingCount == 1) {
            // Try to find the last opening parenthesis
            size_t lastOpenParen = displayText.rfind('(');
            
            if (lastOpenParen != std::string::npos) {
                // Check if there's a function name before this parenthesis
                bool foundFunction = false;
                
                for (const std::string& func : functions) {
                    // Check if there's enough space for the function name
                    if (lastOpenParen >= func.length() && 
                        displayText.substr(lastOpenParen - func.length(), func.length()) == func) {
                        foundFunction = true;
                        break;
                    }
                }
                
                // If we found a function and there's only one unclosed parenthesis,
                // and we're not in a nested expression, suppress the indicator
                if (foundFunction) {
                    // Count parentheses before the last opening one to check for nesting
                    int openBeforeLast = 0, closeBeforeLast = 0;
                    for (size_t i = 0; i < lastOpenParen; i++) {
                        if (displayText[i] == '(') openBeforeLast++;
                        if (displayText[i] == ')') closeBeforeLast++;
                    }
                    
                    // If all previous parentheses are balanced, we're in a simple function call
                    if (openBeforeLast == closeBeforeLast) {
                        shouldShowIndicator = false;
                    }
                }
            }
        }
        
        // Show the indicator if we determined it's necessary
        if (shouldShowIndicator) {
            parenthesesIndicator = " [" + std::to_string(missingCount) + ")]";
            logDebug("Detected " + std::to_string(missingCount) + " unclosed parentheses", "UI");
        }
    }
    
    // Ensure the display text doesn't exceed the display field width
    // If it's too long, show the last part with an ellipsis at the beginning
    int maxDisplayLength = 30; // Adjust based on your display width
    if (displayText.length() > maxDisplayLength) {
        displayText = "..." + displayText.substr(displayText.length() - maxDisplayLength + 3);
    }
    
    SetWindowTextW(hWndDisplay, StringToWString(displayText + parenthesesIndicator).c_str());
}

// Update the memory indicator
void UpdateMemoryIndicator() {
    if (memoryUsed) {
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
        memoryUsed = true;
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
        memoryUsed = true;
        UpdateMemoryIndicator();
        newExpression = true;
    } catch (...) {
        // Handle error
    }
}

void MemoryRecall() {
    if (memoryUsed) {
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
    memoryUsed = false;
    UpdateMemoryIndicator();
}

// Calculate the expression
void CalculateExpression() {
    logDebug("CalculateExpression() called", "CALC");
    
    // If the expression is empty or just "0", do nothing
    if (currentExpression.empty() || currentExpression == "0") {
        logDebug("Expression is empty or just '0', returning without calculation", "CALC");
        return;
    }
    
    // If the expression ends with an operator, handle it
    char lastChar = currentExpression.back();
    std::string debugMsg = "Last character of expression: " + std::string(1, lastChar);
    logDebug(debugMsg, "CALC");
    
    // Special case for percentage at the end
    if (lastChar == '%') {
        logDebug("Processing percentage calculation", "CALC");
        // Assume it's a percentage of the previous result
        currentExpression = currentExpression.substr(0, currentExpression.length() - 1);
        double value = EvaluateExpression(currentExpression);
        value = value / 100.0;
        
        // Format the result
        std::string resultStr = std::to_string(value);
        // Remove trailing zeros
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        if (resultStr.back() == '.') {
            resultStr.pop_back();
        }
        
        // Add to history
        std::string historyEntry = currentExpression + "% = " + resultStr;
        calculationHistory.push_back(historyEntry);
        
        // Set the result as the new expression
        currentExpression = resultStr;
        UpdateDisplay(currentExpression);
        newExpression = true;
        return;
    }
    
    // If the expression ends with an operator (except for %), don't calculate
    if (lastChar == '+' || lastChar == '-' || lastChar == '*' || lastChar == '/') {
        logDebug("Not calculating due to ending with operator", "CALC");
        return;
    }
    
    // Check for balanced parentheses and add closing ones if needed
    logDebug("Checking for matching parentheses", "CALC");
    int openCount = 0, closeCount = 0;
    for (char c : currentExpression) {
        if (c == '(') openCount++;
        if (c == ')') closeCount++;
    }
    
    // Store the original expression for history
    std::string originalExpr = currentExpression;
    
    // If there are unclosed parentheses, add missing closing parentheses
    int addedClosingParenthesis = 0;
    if (openCount > closeCount) {
        logDebug("Adding missing closing parentheses", "CALC");
        addedClosingParenthesis = openCount - closeCount;
        for (int i = 0; i < addedClosingParenthesis; i++) {
            currentExpression += ")";
        }
        // Update the display to show the corrected expression
        UpdateDisplay(currentExpression);
        
        // Log the parentheses addition to debug log only, not to history
        std::string noteMsg = std::to_string(addedClosingParenthesis) + 
                             " closing parentheses were added to " + originalExpr;
        logDebug(noteMsg, "CALC");
        
        // Update originalExpr to include the added parentheses
        originalExpr = currentExpression;
    }
    
    debugMsg = "Starting evaluation of expression: " + originalExpr;
    logDebug(debugMsg, "CALC");
    
    // Evaluate the expression
    double result;
    try {
        result = EvaluateExpression(currentExpression);
        std::string resultMsg = "Result: " + std::to_string(result) + " (from expression: " + originalExpr + ")";
        logDebug(resultMsg, "CALC");
        
        // Format the expression for display (replace * with ×)
        std::string displayExpression = originalExpr;
        size_t pos = 0;
        while ((pos = displayExpression.find('*', pos)) != std::string::npos) {
            displayExpression.replace(pos, 1, "x");
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
        
        logDebug("Updating display with result", "CALC");
        UpdateDisplay(currentExpression);
        newExpression = true;
        
    } catch (const std::exception& e) {
        std::string errorMsg = "Error during calculation: " + std::string(e.what());
        logDebug(errorMsg, "ERROR");
        
        // Add to history
        calculationHistory.push_back(originalExpr + " = Error: " + e.what());
        
        // Update history display
        UpdateHistoryDisplay();
        
        // Improved error recovery: don't clear the expression, allow the user to fix it
        // Check for specific errors and provide helpful messages
        std::string errorStr = e.what();
        if (errorStr.find("Function") != std::string::npos && errorStr.find("requires an argument") != std::string::npos) {
            // Missing function argument
            MessageBoxW(hWndMain, L"Function requires an argument. Please add a value inside the parentheses.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        } 
        else if (errorStr.find("must be between") != std::string::npos) {
            // Out of range argument
            MessageBoxW(hWndMain, L"Function argument is out of valid range. Please check the input value.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        else if (errorStr.find("Unknown identifier") != std::string::npos) {
            // Unknown identifier
            MessageBoxW(hWndMain, L"Unknown identifier found. Please check for typos or invalid characters.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        else if (errorStr.find("Division by zero") != std::string::npos) {
            // Division by zero
            MessageBoxW(hWndMain, L"Cannot divide by zero.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        else {
            // Generic error
            std::wstring wideErrorMsg = StringToWString("Error: " + errorStr);
            MessageBoxW(hWndMain, wideErrorMsg.c_str(), L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        
        // Keep the original expression to allow the user to fix it
        currentExpression = originalExpr;
        UpdateDisplay(currentExpression);
        newExpression = false;
    } catch (...) {
        std::string errorMsg = "Unknown error during calculation";
        logDebug(errorMsg, "ERROR");
        
        // Add to history
        calculationHistory.push_back(originalExpr + " = Error: Unknown error");
        
        // Update history display
        UpdateHistoryDisplay();
        
        // Keep the original expression to allow the user to fix it
        MessageBoxW(hWndMain, L"An unknown error occurred during calculation.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        currentExpression = originalExpr;
        UpdateDisplay(currentExpression);
        newExpression = false;
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
    logDebug("About dialog shown", "UI");
    
    // Create a simple message box as an About dialog - this is NOT for debugging, it's the actual About dialog
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
    return c == '+' || c == '-' || c == '*' || c == 'x' || c == '/' || c == '^' || c == '%';
}

// Get operator precedence
int GetPrecedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 3;
    if (op == '%')
        return 2;  // Same precedence as multiplication/division
    return 0;
}

// Apply an operator to two operands
double ApplyOperator(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': 
        case 'x': // Handle both '*' and 'x' for multiplication
            return a * b;
        case '/': 
            if (b == 0) throw std::runtime_error("Division by zero");
            return a / b;
        case '^': 
            try {
                return calculator.power(a, b);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Power error: ") + e.what());
            }
        case '%': return (a * b) / 100.0; // Percentage of a value
        default: throw std::runtime_error("Unknown operator: " + std::string(1, op));
    }
}

// Helper function to identify functions in expressions
bool IsFunction(const std::string& token) {
    static const std::vector<std::string> functions = {
        "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "sqrt", "log", "ln",
        "abs", "fact"
    };
    
    return std::find(functions.begin(), functions.end(), token) != functions.end();
}

// Helper function to check if a string is a constant
bool IsConstant(const std::string& token) {
    return token == "pi" || token == "e";
}

// Helper function to get value of constants
double GetConstantValue(const std::string& token) {
    if (token == "pi") return calculator.getPi();
    if (token == "e") return calculator.getE();
    throw std::invalid_argument("Unknown constant: " + token);
}

// Parse a function call
double ParseFunction(const std::string& func, const std::string& expr, size_t& i) {
    logDebug("ParseFunction called with function: " + func + ", expr: " + expr + ", i: " + std::to_string(i), "CALC");
    
    // Find the actual position of the function name in the expression
    size_t funcPos = expr.find(func, i > 0 ? i - 1 : 0);
    if (funcPos == std::string::npos) {
        // If not found starting from i, try from the beginning
        funcPos = expr.find(func);
    }
    
    if (funcPos == std::string::npos) {
        std::string errorMsg = "Function " + func + " not found in expression " + expr;
        logDebug(errorMsg, "ERROR");
        throw std::runtime_error("Function not found in expression");
    }
    
    // Position after function name where the opening parenthesis should be
    size_t openParenPos = funcPos + func.length();
    
    // Make sure there's an opening parenthesis
    if (openParenPos >= expr.length() || expr[openParenPos] != '(') {
        std::string errorMsg = "Missing opening parenthesis after function " + func;
        logDebug(errorMsg, "ERROR");
        
        // Try to auto-correct by assuming the next token is the argument
        if (openParenPos < expr.length() && 
            (isdigit(expr[openParenPos]) || expr[openParenPos] == '.' || 
             expr[openParenPos] == 'p' || expr[openParenPos] == 'e')) {
            
            // Create a corrected expression with parentheses
            std::string correctedExpr = expr.substr(0, openParenPos) + "(" + expr.substr(openParenPos);
            
            // Find where to put the closing parenthesis
            size_t closePos = openParenPos + 1;
            while (closePos < correctedExpr.length() && 
                  (isdigit(correctedExpr[closePos]) || correctedExpr[closePos] == '.' || 
                   correctedExpr[closePos] == 'e' || correctedExpr[closePos] == 'E' ||
                   correctedExpr[closePos] == 'p' || correctedExpr[closePos] == 'i')) {
                closePos++;
            }
            
            // Insert closing parenthesis
            correctedExpr.insert(closePos, ")");
            
            logDebug("Auto-corrected function call: " + correctedExpr, "CALC");
            
            // Recursively call with corrected expression
            return EvaluateExpression(correctedExpr);
        }
        
        throw std::runtime_error("Missing opening parenthesis after function");
    }
    
    // Start after the opening parenthesis
    size_t start = openParenPos + 1;
    size_t pos = start;
    
    std::string debugParens = "Parsing function arguments: Start position = " + std::to_string(start) + 
                            ", Expression length = " + std::to_string(expr.length());
    logDebug(debugParens, "CALC");
    
    // Find the matching closing parenthesis
    int parenCount = 1;
    while (pos < expr.length() && parenCount > 0) {
        if (expr[pos] == '(') parenCount++;
        if (expr[pos] == ')') parenCount--;
        pos++;
    }
    
    // Check if we found a matching closing parenthesis
    if (parenCount > 0) {
        std::string errorMsg = "Missing closing parenthesis for function " + func;
        logDebug(errorMsg, "ERROR");
        throw std::runtime_error("Missing closing parenthesis for function");
    }
    
    // Extract the argument string (excluding the closing parenthesis)
    std::string argStr = expr.substr(start, pos - start - 1);
    
    std::string debugArgStr = "Extracted argument: " + argStr;
    logDebug(debugArgStr, "CALC");
    
    // If argument is empty, provide a more helpful error message based on the function
    if (argStr.empty()) {
        std::string errorMsg = "Function " + func + " requires an argument";
        logDebug(errorMsg, "ERROR");
        
        // Provide default values for common functions when possible
        if (func == "sin" || func == "cos" || func == "tan") {
            logDebug("Using default value 0 for empty trigonometric function argument", "CALC");
            argStr = "0";
        } else if (func == "sqrt") {
            logDebug("Using default value 0 for empty square root argument", "CALC");
            argStr = "0";
        } else if (func == "log" || func == "ln") {
            logDebug("Using default value 1 for empty logarithm argument", "CALC");
            argStr = "1";
        } else if (func == "abs") {
            logDebug("Using default value 0 for empty absolute value argument", "CALC");
            argStr = "0";
        } else if (func == "fact") {
            logDebug("Using default value 1 for empty factorial argument", "CALC");
            argStr = "1";
        } else {
            throw std::runtime_error("Function " + func + " requires an argument");
        }
    }
    
    // Update the position to after the closing parenthesis
    i = pos;
    
    // Evaluate the argument
    double argValue = EvaluateExpression(argStr);
    
    // Apply the function
    double result = 0.0;
    if (func == "sin") {
        result = calculator.sine(argValue);
    } else if (func == "cos") {
        result = calculator.cosine(argValue);
    } else if (func == "tan") {
        result = calculator.tangent(argValue);
    } else if (func == "asin") {
        if (argValue < -1.0 || argValue > 1.0) {
            throw std::runtime_error("Arcsine argument must be between -1 and 1");
        }
        result = calculator.arcsine(argValue);
    } else if (func == "acos") {
        if (argValue < -1.0 || argValue > 1.0) {
            throw std::runtime_error("Arccosine argument must be between -1 and 1");
        }
        result = calculator.arccosine(argValue);
    } else if (func == "atan") {
        result = calculator.arctangent(argValue);
    } else if (func == "sinh") {
        result = calculator.sineH(argValue);
    } else if (func == "cosh") {
        result = calculator.cosineH(argValue);
    } else if (func == "tanh") {
        result = calculator.tangentH(argValue);
    } else if (func == "sqrt") {
        if (argValue < 0.0) {
            throw std::runtime_error("Cannot take square root of negative number");
        }
        result = calculator.squareRoot(argValue);
    } else if (func == "log") {
        if (argValue <= 0.0) {
            throw std::runtime_error("Cannot take logarithm of non-positive number");
        }
        result = calculator.logarithm(argValue, 10.0);
    } else if (func == "ln") {
        if (argValue <= 0.0) {
            throw std::runtime_error("Cannot take natural logarithm of non-positive number");
        }
        result = calculator.naturalLogarithm(argValue);
    } else if (func == "abs") {
        result = calculator.absolute(argValue);
    } else if (func == "fact") {
        if (argValue < 0 || std::floor(argValue) != argValue) {
            throw std::runtime_error("Factorial is defined only for non-negative integers");
        }
        result = calculator.factorial(argValue);
    } else {
        throw std::runtime_error("Unknown function: " + func);
    }
    
    return result;
}

// Evaluate a mathematical expression with enhanced capabilities
double EvaluateExpression(const std::string& expression) {
    std::string debugMsg = "EvaluateExpression called with: " + expression;
    logDebug(debugMsg, "CALC");
    
    // Check if expression contains any function calls without proper parentheses
    // This is a common problem with inputs like "cos5" instead of "cos(5)"
    std::vector<std::string> functions = {"sin", "cos", "tan", "asin", "acos", "atan", 
                                        "sinh", "cosh", "tanh", "sqrt", "log", "ln",
                                        "abs", "fact"};
    
    std::string processedExpr = expression;
    
    // Check for function calls without opening parenthesis
    for (const std::string& func : functions) {
        size_t pos = 0;
        while ((pos = processedExpr.find(func, pos)) != std::string::npos) {
            // Make sure we found the function name, not part of another token
            bool isValidFunction = false;
            
            // If it's at the beginning of the string or preceded by an operator or opening parenthesis
            if (pos == 0 || IsOperator(processedExpr[pos-1]) || processedExpr[pos-1] == '(' || 
                processedExpr[pos-1] == '*' || processedExpr[pos-1] == '/' || 
                processedExpr[pos-1] == '+' || processedExpr[pos-1] == '-') {
                isValidFunction = true;
            }
            
            if (isValidFunction) {
                // Check if function is followed by opening parenthesis
                size_t nextPos = pos + func.length();
                if (nextPos < processedExpr.length() && processedExpr[nextPos] != '(') {
                    // No opening parenthesis, so we need to add one
                    // Make sure there's actually something after the function
                    if (nextPos < processedExpr.length() && 
                        (isdigit(processedExpr[nextPos]) || processedExpr[nextPos] == '.' || 
                         processedExpr[nextPos] == 'p' || processedExpr[nextPos] == 'e')) {
                        
                        // Insert opening parenthesis
                        processedExpr.insert(nextPos, "(");
                        
                        // Find the end of the argument
                        size_t argEnd = nextPos + 1;
                        int nestedParens = 0;
                        
                        // Handle more complex arguments like abs(2+3) or fact(pi)
                        while (argEnd < processedExpr.length()) {
                            if (processedExpr[argEnd] == '(') {
                                nestedParens++;
                            } else if (processedExpr[argEnd] == ')') {
                                if (nestedParens == 0) break;
                                nestedParens--;
                            } else if (nestedParens == 0 && IsOperator(processedExpr[argEnd]) && 
                                      argEnd > nextPos + 1) {
                                break;
                            }
                            argEnd++;
                        }
                        
                        // If we reached the end without finding a proper end, use the whole rest of the expression
                        if (argEnd == processedExpr.length()) {
                            processedExpr += ")";
                        } else if (!IsOperator(processedExpr[argEnd]) && processedExpr[argEnd] != ')') {
                            // If we stopped at a non-operator, non-parenthesis character, include it
                            processedExpr.insert(argEnd + 1, ")");
                        } else {
                            // Otherwise insert before the operator or closing parenthesis
                            processedExpr.insert(argEnd, ")");
                        }
                        
                        std::string fixedExpr = "Auto-fixed function '" + func + "' without parentheses: " + processedExpr;
                        logDebug(fixedExpr, "CALC");
                    }
                }
            }
            
            // Move past this occurrence
            pos += func.length();
        }
    }
    
    // If the expression was modified, use the processed expression
    if (processedExpr != expression) {
        logDebug("Using modified expression: " + processedExpr, "CALC");
        return EvaluateExpression(processedExpr);
    }
    
    // If expression is empty, return 0
    if (expression.empty()) {
        logDebug("Expression is empty, returning 0", "CALC");
        return 0.0;
    }
    
    std::stack<double> values;
    std::stack<char> operators;
    std::string currentToken;
    bool expectingOperand = true;
    
    logDebug("Starting to parse expression", "CALC");
    
    for (size_t i = 0; i < expression.length(); i++) {
        // Skip spaces
        if (expression[i] == ' ')
            continue;
        
        // Handle unary minus
        if (expression[i] == '-' && expectingOperand) {
            logDebug("Found unary minus", "CALC");
            values.push(0);    // Add 0 before - to convert to binary operation
            operators.push('-');
            expectingOperand = true;
            continue;
        }
        
        // Handle unary plus (just ignore it)
        if (expression[i] == '+' && expectingOperand) {
            logDebug("Found unary plus, ignoring", "CALC");
            continue;  // Just ignore unary plus
        }
        
        // If current character is an opening bracket, push it to operators stack
        if (expression[i] == '(') {
            operators.push(expression[i]);
            expectingOperand = true;
            continue;
        }
        
        // If current character is a closing bracket, solve the entire bracket
        if (expression[i] == ')') {
            // If we're expecting an operand, there's a syntax error
            if (expectingOperand) {
                throw std::runtime_error("Invalid expression: empty parentheses or missing operand before ')'");
            }
            
            bool foundMatchingParen = false;
            
            while (!operators.empty()) {
                if (operators.top() == '(') {
                    foundMatchingParen = true;
                    operators.pop(); // Remove the '('
                    break;
                }
                
                char op = operators.top(); operators.pop();
                
                if (values.size() < 2) {
                    throw std::runtime_error("Invalid expression: not enough operands for operator '" + std::string(1, op) + "'");
                }
                
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                
                try {
                    values.push(ApplyOperator(val1, val2, op));
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string(e.what()) + " when evaluating " + 
                                            std::to_string(val1) + " " + op + " " + std::to_string(val2));
                }
            }
            
            if (!foundMatchingParen) {
                throw std::runtime_error("Mismatched parentheses: extra ')'");
            }
            
            expectingOperand = false;
            continue;
        }
        
        // Check for functions and constants
        if (isalpha(expression[i])) {
            currentToken.clear();
            
            // Extract function name or constant
            while (i < expression.length() && (isalpha(expression[i]) || isdigit(expression[i]))) {
                currentToken += expression[i++];
            }
            i--; // Move back to the last character of token
            
            // Check if we have a constant (pi or e)
            if (IsConstant(currentToken)) {
                values.push(GetConstantValue(currentToken));
                expectingOperand = false;
                continue;
            }
            
            // Check if we have a function call
            if (IsFunction(currentToken) && i + 1 < expression.length() && expression[i + 1] == '(') {
                try {
                    double functionResult = ParseFunction(currentToken, expression, i);
                    values.push(functionResult);
                    expectingOperand = false;
                    i--; // Adjust for the loop increment
                    continue;
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string(e.what()));
                }
            }
            
            throw std::runtime_error("Unknown identifier: '" + currentToken + "'");
        }
        
        // If current character is an operator
        if (IsOperator(expression[i])) {
            // Can't have operator after another operator (except unary)
            if (expectingOperand) {
                throw std::runtime_error("Invalid expression: operator '" + std::string(1, expression[i]) + 
                                         "' cannot follow another operator");
            }
            
            // While top of operators has same or greater precedence
            while (!operators.empty() && operators.top() != '(' &&
                   GetPrecedence(operators.top()) >= GetPrecedence(expression[i])) {
                char op = operators.top(); operators.pop();
                
                if (values.size() < 2) {
                    throw std::runtime_error("Invalid expression: not enough operands for operator '" + 
                                            std::string(1, op) + "'");
                }
                
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                
                try {
                    values.push(ApplyOperator(val1, val2, op));
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string(e.what()) + " when evaluating " + 
                                            std::to_string(val1) + " " + op + " " + std::to_string(val2));
                }
            }
            
            // Push current operator to stack
            operators.push(expression[i]);
            expectingOperand = true;
            continue;
        }
        
        // If current character is a number or decimal point
        if (isdigit(expression[i]) || expression[i] == '.') {
            std::string currentNumber;
            
            // Extract the complete number
            while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
                currentNumber += expression[i++];
            }
            i--; // Move back one position as the for loop will increment
            
            // Validate the number
            if (std::count(currentNumber.begin(), currentNumber.end(), '.') > 1) {
                throw std::runtime_error("Invalid number format: multiple decimal points in '" + currentNumber + "'");
            }
            
            // Convert string to double and push to values stack
            try {
                values.push(std::stod(currentNumber));
            } catch (...) {
                throw std::runtime_error("Invalid number format: '" + currentNumber + "'");
            }
            
            expectingOperand = false;
            continue;
        }
        
        // If we reach here, the character is not recognized
        throw std::runtime_error("Unrecognized character in expression: '" + std::string(1, expression[i]) + "'");
    }
    
    // Check for incomplete expression (ending with an operator)
    if (expectingOperand && !operators.empty()) {
        throw std::runtime_error("Invalid expression: ends with an operator");
    }
    
    // Process all remaining operators
    while (!operators.empty()) {
        char op = operators.top(); operators.pop();
        
        if (op == '(') {
            throw std::runtime_error("Mismatched parentheses: extra '('");
        }
        
        if (values.size() < 2) {
            throw std::runtime_error("Invalid expression: not enough operands for operator '" + std::string(1, op) + "'");
        }
        
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        
        try {
            values.push(ApplyOperator(val1, val2, op));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(e.what()) + " when evaluating " + 
                                    std::to_string(val1) + " " + op + " " + std::to_string(val2));
        }
    }
    
    // Final result should be on top of the values stack
    if (values.empty()) {
        logDebug("Values stack is empty, error: no operands", "ERROR");
        throw std::runtime_error("Invalid expression: no operands");
    }
    
    if (values.size() != 1) {
        logDebug("Error: too many operands left on stack", "ERROR");
        throw std::runtime_error("Invalid expression: too many operands");
    }
    
    double result = values.top();
    std::string resultMsg = "Final result: " + std::to_string(result);
    logDebug(resultMsg, "CALC");
    
    return result;
}

// Function to switch to the next theme
void SwitchTheme() {
    // Cycle to the next theme
    currentThemeIndex = (currentThemeIndex + 1) % availableThemes.size();
    currentTheme = &availableThemes[currentThemeIndex];
    
    // Show theme change message
    std::string themeName = WStringToString(currentTheme->name);
    logDebug("Theme changed to: " + themeName, "UI");
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

// Helper function to check if a string is a valid identifier
bool IsValidIdentifier(const std::string& token) {
    // Check if it's a function
    if (IsFunction(token)) return true;
    
    // Check if it's a constant
    if (IsConstant(token)) return true;
    
    // Check if it's a number (can start with a digit or decimal point)
    if (!token.empty() && (isdigit(token[0]) || token[0] == '.')) {
        bool hasDecimal = false;
        for (size_t i = 0; i < token.length(); i++) {
            if (token[i] == '.') {
                if (hasDecimal) return false; // Multiple decimal points
                hasDecimal = true;
            } else if (!isdigit(token[i])) {
                return false; // Non-digit character
            }
        }
        return true;
    }
    
    // Not a valid identifier
    return false;
}

// Helper function to extract the last token from an expression
std::string GetLastToken(const std::string& expr) {
    if (expr.empty()) return "";
    
    // Find the last non-alphanumeric character (operator or parenthesis)
    size_t lastNonAlphaNumPos = expr.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.");
    
    // If not found or it's the last character, there's no token
    if (lastNonAlphaNumPos == std::string::npos) {
        return expr; // The entire expression is one token
    } else if (lastNonAlphaNumPos == expr.length() - 1) {
        return ""; // The last character is an operator or parenthesis
    }
    
    // Extract the token after the last non-alphanumeric character
    return expr.substr(lastNonAlphaNumPos + 1);
}

// Format expression with visual cues for operator precedence
std::string FormatExpressionWithPrecedence(const std::string& expr) {
    std::string result = expr;
    
    // Replace * with × for display
    size_t pos = 0;
    while ((pos = result.find('*', pos)) != std::string::npos) {
        result.replace(pos, 1, "x");
        pos += 1;
    }
    
    // Add subtle visual cues for operator precedence
    // First, find all operators and their positions
    std::vector<std::pair<size_t, char>> operators;
    for (size_t i = 0; i < result.length(); i++) {
        char currentChar = result[i];
        if (currentChar == '+' || currentChar == '-' || 
            currentChar == 'x' || currentChar == '/' || 
            currentChar == '^' || currentChar == '%') {
            operators.push_back({i, currentChar});
        }
    }
    
    // Process operators from highest to lowest precedence
    // First pass: add spaces around operators based on precedence
    for (auto it = operators.rbegin(); it != operators.rend(); ++it) {
        size_t pos = it->first;
        char op = it->second;
        
        // Skip if this is a unary operator
        if (pos == 0 || result[pos-1] == '(' || 
            (pos > 0 && (result[pos-1] == '+' || result[pos-1] == '-' || 
                         result[pos-1] == 'x' || result[pos-1] == '/' || 
                         result[pos-1] == '^' || result[pos-1] == '%'))) {
            continue;
        }
        
        // Get precedence of this operator
        int precedence = 0;
        if (op == '+' || op == '-') precedence = 1;
        else if (op == 'x' || op == '/' || op == '%') precedence = 2;
        else if (op == '^') precedence = 3;
        
        // Add spaces based on precedence (higher precedence = fewer spaces)
        std::string replacement;
        switch (precedence) {
            case 1: replacement = " " + std::string(1, op) + " "; break;  // Lowest precedence
            case 2: replacement = " " + std::string(1, op) + " "; break;  // Medium precedence
            case 3: replacement = " " + std::string(1, op) + " "; break;  // Highest precedence
            default: replacement = std::string(1, op); break;
        }
        
        // Replace the operator with the formatted version
        result.replace(pos, 1, replacement);
        
        // Update positions of subsequent operators
        for (auto jt = it.base(); jt != operators.end(); ++jt) {
            if (jt->first > pos) {
                jt->first += replacement.length() - 1;
            }
        }
    }
    
    return result;
}
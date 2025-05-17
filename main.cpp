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
#include <algorithm>
#include "calculator.h"
#include <fstream>
#include <iomanip>
#include <ctime>

#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR (WM_USER + 67)
#endif

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
    COLORREF numericButtonText;
    COLORREF operatorButtonText;
    COLORREF equalButtonText;
    COLORREF clearButtonText;
    COLORREF memoryButtonText;
    COLORREF specialButtonText;
    COLORREF utilityButtonText;
};

std::vector<ThemeColors> availableThemes = {
    {
        L"Default",
        RGB(240, 240, 240),
        RGB(255, 255, 255),
        RGB(0, 0, 0),
        RGB(240, 240, 240),
        RGB(173, 216, 230),
        RGB(144, 238, 144),
        RGB(255, 128, 128),
        RGB(255, 128, 128),
        RGB(173, 216, 230),
        RGB(255, 255, 255),
        RGB(0, 0, 0),
        RGB(0, 0, 0),
        RGB(0, 0, 0),
        RGB(0, 60, 0),
        RGB(60, 0, 0),
        RGB(60, 0, 0),
        RGB(0, 0, 80),
        RGB(0, 0, 0)
    },
    {
        L"Dark",
        RGB(50, 50, 50),
        RGB(30, 30, 30),
        RGB(220, 220, 220),
        RGB(70, 70, 70),
        RGB(100, 100, 140),
        RGB(70, 140, 70),
        RGB(140, 70, 70),
        RGB(140, 70, 70),
        RGB(70, 110, 140),
        RGB(40, 40, 40),
        RGB(200, 200, 200),
        RGB(220, 220, 220),
        RGB(240, 240, 240),
        RGB(230, 255, 230),
        RGB(255, 230, 230),
        RGB(255, 230, 230),
        RGB(220, 235, 255),
        RGB(220, 220, 220)
    },
    {
        L"Blue",
        RGB(230, 240, 250),
        RGB(240, 250, 255),
        RGB(0, 30, 60),
        RGB(200, 220, 240),
        RGB(150, 180, 210),
        RGB(120, 200, 170),
        RGB(240, 150, 150),
        RGB(180, 180, 220),
        RGB(150, 180, 210),
        RGB(240, 250, 255),
        RGB(0, 30, 60),
        RGB(0, 30, 60),
        RGB(0, 30, 80),
        RGB(0, 60, 40),
        RGB(100, 0, 0),
        RGB(40, 40, 100),
        RGB(0, 40, 80),
        RGB(30, 30, 30)
    }
};

int currentThemeIndex = 0;
ThemeColors* currentTheme = &availableThemes[currentThemeIndex];

HBRUSH themeBackgroundBrush = NULL;
HBRUSH buttonBrushes[10] = {NULL};

std::ofstream debugLogFile;

void initDebugLog() {
    debugLogFile.open("calculator_debug.log", std::ios::out | std::ios::trunc);
    
    time_t now = time(0);
    char timeStr[26];
    ctime_s(timeStr, sizeof(timeStr), &now);
    
    debugLogFile << "------------------------------------------------------------------------------" << std::endl;
    debugLogFile << "Calculator Debug Log - Started: " << timeStr;
    debugLogFile << "------------------------------------------------------------------------------" << std::endl;
    debugLogFile << "Build version: " << __DATE__ << " " << __TIME__ << std::endl;
    debugLogFile << std::endl;
    
    debugLogFile << std::fixed << std::setprecision(6);
    
    debugLogFile.flush();
}

void closeDebugLog() {
    if (debugLogFile.is_open()) {
        time_t now = time(0);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &now);
        
        debugLogFile << std::endl;
        debugLogFile << "------------------------------------------------------------------------------" << std::endl;
        debugLogFile << "Calculator Debug Log - Ended: " << timeStr;
        debugLogFile << "------------------------------------------------------------------------------" << std::endl;
        
        debugLogFile.close();
    }
}

void logDebug(const std::string& message, const std::string& category = "INFO") {
    if (debugLogFile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        
        debugLogFile << "[" 
                   << std::setw(2) << std::setfill('0') << st.wHour << ":"
                   << std::setw(2) << std::setfill('0') << st.wMinute << ":" 
                   << std::setw(2) << std::setfill('0') << st.wSecond << "." 
                   << std::setw(3) << std::setfill('0') << st.wMilliseconds << "] "
                   << "[" << category << "] " << message << std::endl;
        
        debugLogFile.flush();
    }
}

std::wstring StringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string WStringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

HWND hWndMain;
HWND hWndDisplay;
HWND hWndMemoryIndicator;
HWND hWndHistoryList;
HWND hWndButtons[45];
HFONT hDisplayFont = NULL;
HFONT hHistoryFont = NULL;
HFONT hButtonFonts[45] = { NULL };
Calculator calculator;
std::string currentExpression = "0";
std::string previousExpression = "0";
bool newExpression = true;
double memoryValue = 0.0;
bool memoryUsed = false;
std::vector<std::string> calculationHistory;

struct ButtonDef {
    const wchar_t* label;
    int x;
    int y;
    int width;
    int height;
    COLORREF bgColor;
};

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
void CleanupResources();
std::string FormatExpressionWithPrecedence(const std::string& expr);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    initDebugLog();
    logDebug("Application started", "MAIN");
    
    const wchar_t CLASS_NAME[] = L"CalculatorWindowClass";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClassW(&wc);
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    int windowWidth = 520;
    int windowHeight = 680;
    
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;
    
    hWndMain = CreateWindowExW(
        0,                         
        CLASS_NAME,                
        L"Advanced C++ Calculator", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        
        windowX, windowY, windowWidth, windowHeight,
        
        NULL,      
        NULL,      
        hInstance, 
        NULL       
    );
    
    if (hWndMain == NULL) {
        return 0;
    }
    
    CreateCalculatorUI(hWndMain);
    
    UpdateMemoryIndicator();
    
    UpdateHistoryDisplay();
    
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    logDebug("Application ending", "MAIN");
    closeDebugLog();
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            return 0;
            
        case WM_KEYDOWN: {
            int keyCode = static_cast<int>(wParam);
            bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            
            wchar_t debugMsg[100];
            swprintf(debugMsg, 100, L"Key pressed: %d, Shift: %d, Ctrl: %d", keyCode, shiftPressed, ctrlPressed);
            OutputDebugStringW(debugMsg);
            
            if ((keyCode >= '0' && keyCode <= '9') || 
                (keyCode >= VK_NUMPAD0 && keyCode <= VK_NUMPAD9)) {
                if (!shiftPressed || (keyCode != '6' && keyCode != '8')) {
                    wchar_t keyChar;
                    if (keyCode >= '0' && keyCode <= '9') {
                        keyChar = static_cast<wchar_t>(keyCode);
                    } else {
                        keyChar = static_cast<wchar_t>('0' + (keyCode - VK_NUMPAD0));
                    }
                    wchar_t buttonText[2] = { keyChar, L'\0' };
                    HandleButtonClick(buttonText);
                    return 0;
                }
            }
            
            if (keyCode == VK_DECIMAL || (keyCode == VK_OEM_PERIOD && !shiftPressed)) {
                HandleButtonClick(L".");
                return 0;
            }
            
            if (keyCode == VK_OEM_PLUS || keyCode == '=') {
                if (shiftPressed) {
                    HandleButtonClick(L"+");
                } else {
                    HandleButtonClick(L"=");
                }
                return 0;
            }
            
            if (keyCode == VK_ADD) {
                HandleButtonClick(L"+");
                return 0;
            }
            
            if (keyCode == VK_SUBTRACT || keyCode == VK_OEM_MINUS) {
                HandleButtonClick(L"-");
                return 0;
            }
            
            if (keyCode == '8') {
                if (shiftPressed) {
                    HandleButtonClick(L"x");
                } else {
                    HandleButtonClick(L"8");
                }
                return 0;
            }
            
            if (keyCode == VK_MULTIPLY || keyCode == 'X' || keyCode == 'x') {
                HandleButtonClick(L"x");
                return 0;
            }
            
            if (keyCode == VK_DIVIDE || keyCode == VK_OEM_2 || 
                (keyCode == '/' && !shiftPressed)) {
                HandleButtonClick(L"/");
                return 0;
            }
            
            if (keyCode == '6') {
                if (shiftPressed) {
                    HandleButtonClick(L"^");
                } else {
                    HandleButtonClick(L"6");
                }
                return 0;
            }
            
            if (keyCode == '^') {
                HandleButtonClick(L"^");
                return 0;
            }
            
            if (keyCode == '%' || (keyCode == '5' && shiftPressed)) {
                HandleButtonClick(L"%");
                return 0;
            }
            
            if (keyCode == VK_RETURN) {
                HandleButtonClick(L"=");
                return 0;
            }
            
            if (keyCode == VK_ESCAPE) {
                HandleButtonClick(L"C");
                return 0;
            }
            
            if (keyCode == VK_BACK) {
                HandleButtonClick(L"DEL");
                return 0;
            }
            
            if (keyCode == 'S' || keyCode == 's') {
                HandleButtonClick(L"sqrt");
                return 0;
            }
            
            if (keyCode == 'L' || keyCode == 'l') {
                if (shiftPressed) {
                    HandleButtonClick(L"log");
                } else {
                    HandleButtonClick(L"ln");
                }
                return 0;
            }
            
            if (keyCode == 'M' || keyCode == 'm') {
                if (shiftPressed && ctrlPressed) {
                    HandleButtonClick(L"MC");
                } else if (shiftPressed) {
                    HandleButtonClick(L"M-");
                } else if (ctrlPressed) {
                    HandleButtonClick(L"MR");
                } else {
                    HandleButtonClick(L"M+");
                }
                return 0;
            }
            
            return 0;
        }
        
        case WM_CHAR: {
            int charCode = static_cast<int>(wParam);
            
            wchar_t debugMsg[100];
            swprintf(debugMsg, 100, L"Char received: %d", charCode);
            OutputDebugStringW(debugMsg);
            
            if (charCode == '^') {
                HandleButtonClick(L"^");
                return 0;
            }
            else if (charCode == '*') {
                HandleButtonClick(L"x");
                return 0;
            }
            
            return 0;
        }
        
        case WM_COMMAND: {
            int buttonId = LOWORD(wParam);
            
            std::string debugMsg = "WM_COMMAND received, buttonId: " + std::to_string(buttonId);
            logDebug(debugMsg, "UI");
            
            if (buttonId >= 1000 && buttonId <= 1050) {
                HWND buttonHwnd = (HWND)lParam;
                wchar_t buttonText[50];
                GetWindowTextW(buttonHwnd, buttonText, 50);
                
                std::string narrowBtnText = WStringToString(buttonText);
                std::string btnClickMsg = "Button clicked in WindowProc: " + narrowBtnText;
                logDebug(btnClickMsg, "UI");
                
                if (wcscmp(buttonText, L"THEME") == 0) {
                    logDebug("Theme button detected in WindowProc", "UI");
                    SwitchTheme();
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
                    return 0;
                }
                
                if (wcscmp(buttonText, L"UNDO") == 0) {
                    logDebug("UNDO button detected in WindowProc", "UI");
                    HandleButtonClick(buttonText);
                    return 0;
                }
                
                logDebug("Regular button detected in WindowProc", "UI");
                HandleButtonClick(buttonText);
            }
            break;
        }
        
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            
            if (dis->CtlType == ODT_BUTTON) {
                int buttonIndex = -1;
                for (int i = 0; i < 45; i++) {
                    if (dis->hwndItem == hWndButtons[i]) {
                        buttonIndex = i;
                        break;
                    }
                }
                
                if (buttonIndex >= 0) {
                    wchar_t buttonText[20];
                    GetWindowTextW(dis->hwndItem, buttonText, sizeof(buttonText)/sizeof(wchar_t));
                    
                    COLORREF bgColor = currentTheme->numericButtonBackground;
                    COLORREF txtColor = currentTheme->numericButtonText;
                    
                    if (wcscmp(buttonText, L"C") == 0) {
                        bgColor = currentTheme->clearButtonBackground;
                        txtColor = currentTheme->clearButtonText;
                    } else if (wcscmp(buttonText, L"=") == 0) {
                        bgColor = currentTheme->equalButtonBackground;
                        txtColor = currentTheme->equalButtonText;
                    } else if (wcscmp(buttonText, L"+") == 0 || wcscmp(buttonText, L"-") == 0 ||
                               wcscmp(buttonText, L"x") == 0 || wcscmp(buttonText, L"/") == 0 ||
                               wcscmp(buttonText, L"%") == 0) {
                        bgColor = currentTheme->operatorButtonBackground;
                        txtColor = currentTheme->operatorButtonText;
                    } else if (wcscmp(buttonText, L"M+") == 0 || wcscmp(buttonText, L"M-") == 0 ||
                               wcscmp(buttonText, L"MC") == 0 || wcscmp(buttonText, L"MR") == 0) {
                        bgColor = currentTheme->memoryButtonBackground;
                        txtColor = currentTheme->memoryButtonText;
                    } else if (wcscmp(buttonText, L"sqrt") == 0 || wcscmp(buttonText, L"^") == 0 ||
                               wcscmp(buttonText, L"ln") == 0 || wcscmp(buttonText, L"log") == 0 ||
                               wcscmp(buttonText, L"sin") == 0 || wcscmp(buttonText, L"cos") == 0 ||
                               wcscmp(buttonText, L"tan") == 0 || wcscmp(buttonText, L"asin") == 0 ||
                               wcscmp(buttonText, L"acos") == 0 || wcscmp(buttonText, L"atan") == 0 ||
                               wcscmp(buttonText, L"fact") == 0 || wcscmp(buttonText, L"abs") == 0 ||
                               wcscmp(buttonText, L"UNDO") == 0 || wcscmp(buttonText, L"pi") == 0 ||
                               wcscmp(buttonText, L"e") == 0 || wcscmp(buttonText, L"(") == 0 ||
                               wcscmp(buttonText, L")") == 0) {
                        bgColor = currentTheme->specialButtonBackground;
                        txtColor = currentTheme->specialButtonText;
                    } else if (wcscmp(buttonText, L"Theme") == 0) {
                        bgColor = currentTheme->specialButtonBackground;
                        txtColor = currentTheme->utilityButtonText;
                    } else if (wcscmp(buttonText, L"About") == 0) {
                        bgColor = currentTheme->specialButtonBackground;
                        txtColor = currentTheme->utilityButtonText;
                    } else if (wcscmp(buttonText, L"DEL") == 0) {
                        bgColor = currentTheme->clearButtonBackground;
                        txtColor = currentTheme->clearButtonText;
                    }
                    
                    HBRUSH hBrush = CreateSolidBrush(bgColor);
                    
                    FillRect(dis->hDC, &dis->rcItem, hBrush);
                    DeleteObject(hBrush);
                    
                    if (dis->itemState & ODS_SELECTED) {
                        DrawEdge(dis->hDC, &dis->rcItem, EDGE_SUNKEN, BF_RECT);
                    } else {
                        DrawEdge(dis->hDC, &dis->rcItem, EDGE_RAISED, BF_RECT);
                    }
                    
                    SetBkMode(dis->hDC, TRANSPARENT);
                    SetTextColor(dis->hDC, txtColor);
                    
                    RECT textRect = dis->rcItem;
                    DrawTextW(dis->hDC, buttonText, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    
                    if (dis->itemState & ODS_FOCUS) {
                        RECT focusRect = dis->rcItem;
                        focusRect.left += 3;
                        focusRect.top += 3;
                        focusRect.right -= 3;
                        focusRect.bottom -= 3;
                        DrawFocusRect(dis->hDC, &focusRect);
                    }
                    
                    return TRUE;
                }
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
            CleanupResources();
            closeDebugLog();
            PostQuitMessage(0);
            return 0;
            
        case WM_CTLCOLORBTN: {
            HDC hdcBtn = (HDC)wParam;
            HWND hwndBtn = (HWND)lParam;
            
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void CreateCalculatorUI(HWND hwnd) {
    std::vector<ButtonDef> buttons = {
        {L"sin", 20, 100, 50, 40, currentTheme->specialButtonBackground},
        {L"cos", 80, 100, 50, 40, currentTheme->specialButtonBackground},
        {L"tan", 140, 100, 50, 40, currentTheme->specialButtonBackground},
        {L"pi", 200, 100, 40, 40, currentTheme->specialButtonBackground},
        {L"e", 250, 100, 40, 40, currentTheme->specialButtonBackground},
        
        {L"asin", 20, 150, 50, 40, currentTheme->specialButtonBackground},
        {L"acos", 80, 150, 50, 40, currentTheme->specialButtonBackground},
        {L"atan", 140, 150, 50, 40, currentTheme->specialButtonBackground},
        {L"(", 200, 150, 40, 40, currentTheme->operatorButtonBackground},
        {L")", 250, 150, 40, 40, currentTheme->operatorButtonBackground},
        
        {L"C", 20, 200, 60, 50, currentTheme->clearButtonBackground},
        {L"sqrt", 90, 200, 60, 50, currentTheme->specialButtonBackground},
        {L"^", 160, 200, 60, 50, currentTheme->specialButtonBackground},
        {L"/", 230, 200, 60, 50, currentTheme->operatorButtonBackground},
        
        {L"7", 20, 260, 60, 50, currentTheme->numericButtonBackground},
        {L"8", 90, 260, 60, 50, currentTheme->numericButtonBackground},
        {L"9", 160, 260, 60, 50, currentTheme->numericButtonBackground},
        {L"x", 230, 260, 60, 50, currentTheme->operatorButtonBackground},
        
        {L"4", 20, 320, 60, 50, currentTheme->numericButtonBackground},
        {L"5", 90, 320, 60, 50, currentTheme->numericButtonBackground},
        {L"6", 160, 320, 60, 50, currentTheme->numericButtonBackground},
        {L"-", 230, 320, 60, 50, currentTheme->operatorButtonBackground},
        
        {L"1", 20, 380, 60, 50, currentTheme->numericButtonBackground},
        {L"2", 90, 380, 60, 50, currentTheme->numericButtonBackground},
        {L"3", 160, 380, 60, 50, currentTheme->numericButtonBackground},
        {L"+", 230, 380, 60, 50, currentTheme->operatorButtonBackground},
        
        {L"0", 20, 440, 60, 50, currentTheme->numericButtonBackground},
        {L".", 90, 440, 60, 50, currentTheme->numericButtonBackground},
        {L"%", 160, 440, 60, 50, currentTheme->operatorButtonBackground},
        {L"=", 230, 440, 60, 50, currentTheme->equalButtonBackground},
        
        {L"M+", 20, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"M-", 80, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"MR", 140, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"MC", 200, 500, 50, 40, currentTheme->memoryButtonBackground},
        {L"ln", 260, 500, 50, 40, currentTheme->specialButtonBackground},
        
        {L"log", 20, 550, 60, 40, currentTheme->specialButtonBackground},
        {L"fact", 90, 550, 60, 40, currentTheme->specialButtonBackground},
        {L"abs", 160, 550, 60, 40, currentTheme->specialButtonBackground},
        {L"Theme", 230, 550, 80, 40, currentTheme->specialButtonBackground},
        
        {L"About", 20, 600, 80, 40, currentTheme->specialButtonBackground},
        {L"DEL", 110, 600, 80, 40, currentTheme->clearButtonBackground},
        {L"UNDO", 200, 600, 80, 40, currentTheme->specialButtonBackground}
    };
    
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(currentTheme->windowBackground));
    
    hWndMemoryIndicator = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        300, 20, 20, 25,
        hwnd, (HMENU)997, GetModuleHandle(NULL), NULL
    );
    
    hWndDisplay = CreateWindowExW(
        0, L"EDIT", L"0",
        WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_READONLY,
        20, 20, 280, 60,
        hwnd, (HMENU)999, GetModuleHandle(NULL), NULL
    );
    
    SendMessage(hWndDisplay, EM_SETBKGNDCOLOR, 0, currentTheme->displayBackground);
    SendMessage(hWndDisplay, WM_CTLCOLOREDIT, 0, (LPARAM)currentTheme->displayText);
    
    hWndHistoryList = CreateWindowExW(
        0, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
        320, 20, 160, 620,
        hwnd, (HMENU)996, GetModuleHandle(NULL), NULL
    );
    
    SendMessage(hWndHistoryList, EM_SETBKGNDCOLOR, 0, currentTheme->historyBackground);
    
    hHistoryFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hWndHistoryList, WM_SETFONT, (WPARAM)hHistoryFont, TRUE);
    
    hDisplayFont = CreateFontW(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(hWndDisplay, WM_SETFONT, (WPARAM)hDisplayFont, TRUE);
    
    for (size_t i = 0; i < buttons.size(); i++) {
        const ButtonDef& btn = buttons[i];
        hWndButtons[i] = CreateWindowExW(
            0, L"BUTTON", btn.label,
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            btn.x, btn.y, btn.width, btn.height,
            hwnd, (HMENU)(1000 + i), GetModuleHandle(NULL), NULL
        );
        
        hButtonFonts[i] = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        SendMessage(hWndButtons[i], WM_SETFONT, (WPARAM)hButtonFonts[i], TRUE);
    }
}

void HandleButtonClick(const wchar_t* buttonText) {
    std::string narrowBtnText = WStringToString(buttonText);
    std::string debugMsg = "HandleButtonClick called with button: " + narrowBtnText;
    
    if (wcscmp(buttonText, L"=") == 0) {
        logDebug("Equals button detected, calling CalculateExpression()", "CALC");
        std::string debugExpr = "Current expression before calculation: " + currentExpression;
        logDebug(debugExpr, "CALC");
        CalculateExpression();
        return;
    }
    
    if (wcscmp(buttonText, L"pi") == 0) {
        if (newExpression) {
            currentExpression = "pi";
            newExpression = false;
        } else if (currentExpression == "0") {
            currentExpression = "pi";
        } else {
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "pi";
            } else {
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
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "e";
            } else {
                currentExpression += "*e";
            }
        }
        UpdateDisplay(currentExpression);
        return;
    }
    
    if (wcscmp(buttonText, L"(") == 0) {
        if (newExpression) {
            currentExpression = "(";
            newExpression = false;
        } else if (currentExpression == "0") {
            currentExpression = "(";
        } else {
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "(";
            } else {
                currentExpression += "*(";
            }

        }

        UpdateDisplay(currentExpression);
        return;
    }
    
    if (wcscmp(buttonText, L")") == 0) {
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
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += narrowBtnText + "(";
            } else {
                currentExpression += "*" + narrowBtnText + "(";
            }
        }
        
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
    
    if (wcscmp(buttonText, L"sqrt") == 0) {
        logDebug("Square root function clicked: sqrt", "UI");
        
        if (newExpression || currentExpression == "0") {
            currentExpression = "sqrt(";
            newExpression = false;
        } else {
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += "sqrt(";
            } else {
                currentExpression += "*sqrt(";
            }
        }
        
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
    
    if (wcscmp(buttonText, L"DEL") == 0) {
        if (currentExpression.length() > 0 && !newExpression && currentExpression != "0") {
            previousExpression = currentExpression;
            logDebug("Saved expression for undo: " + previousExpression, "UI");
            
            std::string debugMsg = "DEL button pressed, current expression: " + currentExpression;
            logDebug(debugMsg, "UI");
            
            std::vector<std::string> functions = {"sin", "cos", "tan", "asin", "acos", "atan", 
                                                "sinh", "cosh", "tanh", "sqrt", "log", "ln",
                                                "abs", "fact"};
            
            bool deletedFunction = false;
            for (const std::string& func : functions) {
                if (currentExpression.length() >= func.length() + 1 && 
                    currentExpression.substr(currentExpression.length() - (func.length() + 1)) == func + "(") {
                    currentExpression.erase(currentExpression.length() - (func.length() + 1));
                    deletedFunction = true;
                    logDebug("Deleted function: " + func + "(", "UI");
                    break;
                }
            }
            
            if (!deletedFunction) {
                if (currentExpression.length() >= 2 && currentExpression.substr(currentExpression.length() - 2) == "pi") {
                    currentExpression.erase(currentExpression.length() - 2);
                    logDebug("Deleted constant: pi", "UI");
                } else if (currentExpression.length() >= 1 && currentExpression.back() == 'e') {
                    currentExpression.pop_back();
                    logDebug("Deleted constant: e", "UI");
                } else {
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
    
    if (wcscmp(buttonText, L"UNDO") == 0 || wcscmp(buttonText, L"Ctrl+Z") == 0) {
        if (previousExpression != "0") {
            std::string temp = currentExpression;
            currentExpression = previousExpression;
            previousExpression = temp;
            
            logDebug("Undo performed, restored: " + currentExpression, "UI");
            UpdateDisplay(currentExpression);
        }
        return;
    }
    
    if (isdigit(narrowBtnText[0]) || (narrowBtnText[0] == '.' && narrowBtnText[1] == '\0')) {
        if (newExpression) {
            currentExpression = narrowBtnText;
            newExpression = false;
        } else {
            if (narrowBtnText[0] == '.') {
                size_t lastOpPos = currentExpression.find_last_of("+-*/^%()");
                if (lastOpPos == std::string::npos) {
                    lastOpPos = 0;
                } else {
                }
                
                std::string lastNumber = currentExpression.substr(lastOpPos);
                if (lastNumber.find('.') != std::string::npos) {
                }
            }
            currentExpression += narrowBtnText;
        }
        UpdateDisplay(currentExpression);
    }
    else if (wcscmp(buttonText, L"+") == 0 || wcscmp(buttonText, L"-") == 0 || 
             wcscmp(buttonText, L"x") == 0 || wcscmp(buttonText, L"/") == 0 || 
             wcscmp(buttonText, L"^") == 0 || wcscmp(buttonText, L"%") == 0) {
        
        if (currentExpression.empty() || currentExpression == "0") {
            if (wcscmp(buttonText, L"-") == 0) {
                currentExpression = "-";
                newExpression = false;
                UpdateDisplay(currentExpression);
            }
            return;
        }
        
        std::string opStr = narrowBtnText;
        if (opStr == "x") {
            opStr = "*";
        }
        
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
            char lastChar = currentExpression.back();
            if (IsOperator(lastChar) || lastChar == '(') {
                currentExpression += funcName + "(";
            } else {
                currentExpression += "*" + funcName + "(";
            }
        }
        
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
        logDebug("Theme button clicked, switching theme", "UI");
        SwitchTheme();
        ApplyTheme(hWndMain);
    }
    else if (wcscmp(buttonText, L"About") == 0) {
        logDebug("About button clicked, showing dialog", "UI");
        ShowAboutDialog(hWndMain);
    }
    else if (wcscmp(buttonText, L"C") == 0) {
        ClearCalculator();
    }
    else if (wcscmp(buttonText, L"M+") == 0) {
        MemoryAdd();
    }
    else if (wcscmp(buttonText, L"M-") == 0) {
        MemorySubtract();
    }
    else if (wcscmp(buttonText, L"MR") == 0) {
        MemoryRecall();
    }
    else if (wcscmp(buttonText, L"MC") == 0) {
        MemoryClear();
    }
}

void UpdateDisplay(const std::string& text) {
    std::string displayText = text;
    if (displayText.empty()) {
        displayText = "0";
    }
    
    std::string formattedText = FormatExpressionWithPrecedence(displayText);
    
    int unclosedParentheses = 0;
    for (char c : displayText) {
        if (c == '(') {
            unclosedParentheses++;
        } else if (c == ')') {
            unclosedParentheses--;
        }
    }
    
    if (unclosedParentheses > 0) {
        bool showIndicator = true;
        
        std::vector<std::string> functions = {"sin", "cos", "tan", "asin", "acos", "atan", "log", "ln", "sqrt", "fact", "abs"};
        
        if (unclosedParentheses == 1) {
            size_t lastOpenParenPos = displayText.find_last_of('(');
            
            if (lastOpenParenPos != std::string::npos && lastOpenParenPos > 0) {
                for (const auto& func : functions) {
                    if (lastOpenParenPos >= func.length()) {
                        std::string potentialFunction = displayText.substr(lastOpenParenPos - func.length(), func.length());
                        if (potentialFunction == func) {
                            int parenthesesBefore = 0;
                            for (size_t i = 0; i < lastOpenParenPos - func.length(); i++) {
                                if (displayText[i] == '(') parenthesesBefore++;
                                if (displayText[i] == ')') parenthesesBefore--;
                            }
                            
                            if (parenthesesBefore == 0) {
                                showIndicator = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        if (showIndicator) {
            formattedText += " (";
            for (int i = 0; i < unclosedParentheses; i++) {
                formattedText += ")";
            }
        }
    }
    
    int maxDisplayLength = 30;
    if (displayText.length() > maxDisplayLength) {
        formattedText = "..." + formattedText.substr(formattedText.length() - maxDisplayLength);
    }
    
    SetWindowTextW(hWndDisplay, StringToWString(formattedText).c_str());
}

void UpdateMemoryIndicator() {
    if (memoryUsed) {
        SetWindowTextW(hWndMemoryIndicator, L"M");
    } else {
        SetWindowTextW(hWndMemoryIndicator, L"");
    }
}

void UpdateHistoryDisplay() {
    std::wstring historyText = L"Calculation History:\r\n";
    
    for (int i = calculationHistory.size() - 1; i >= 0; i--) {
        std::wstring historyItem = StringToWString(calculationHistory[i]);
        
        historyText += historyItem + L"\r\n";
    }
    
    SetWindowTextW(hWndHistoryList, historyText.c_str());
}

void MemoryAdd() {
    try {
        double expressionValue = EvaluateExpression(currentExpression);
        memoryValue += expressionValue;
        memoryUsed = true;
        UpdateMemoryIndicator();
        newExpression = true;
    } catch (...) {
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
    }
}

void MemoryRecall() {
    if (memoryUsed) {
        if (newExpression) {
            currentExpression = std::to_string(memoryValue);
            currentExpression.erase(currentExpression.find_last_not_of('0') + 1, std::string::npos);
            if (currentExpression.back() == '.') {
                currentExpression.pop_back();
            }
            newExpression = false;
        } else {
            if (IsOperator(currentExpression.back())) {
                std::string memStr = std::to_string(memoryValue);
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

void CalculateExpression() {
    logDebug("CalculateExpression() called", "CALC");
    
    if (currentExpression.empty() || currentExpression == "0") {
        logDebug("Expression is empty or just '0', returning without calculation", "CALC");
        return;
    }
    
    char lastChar = currentExpression.back();
    std::string debugMsg = "Last character of expression: " + std::string(1, lastChar);
    logDebug(debugMsg, "CALC");
    
    if (lastChar == '%') {
        logDebug("Processing percentage calculation", "CALC");
        currentExpression = currentExpression.substr(0, currentExpression.length() - 1);
        double value = EvaluateExpression(currentExpression);
        value = value / 100.0;
        
        std::string resultStr = std::to_string(value);
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        if (resultStr.back() == '.') {
            resultStr.pop_back();
        }
        
        std::string historyEntry = currentExpression + "% = " + resultStr;
        calculationHistory.push_back(historyEntry);
        
        currentExpression = resultStr;
        UpdateDisplay(currentExpression);
        newExpression = true;
        return;
    }
    
    if (lastChar == '+' || lastChar == '-' || lastChar == '*' || lastChar == '/') {
        logDebug("Not calculating due to ending with operator", "CALC");
        return;
    }
    
    logDebug("Checking for matching parentheses", "CALC");
    int openCount = 0, closeCount = 0;
    for (char c : currentExpression) {
        if (c == '(') openCount++;
        if (c == ')') closeCount++;
    }
    
    std::string originalExpr = currentExpression;
    
    int addedClosingParenthesis = 0;
    if (openCount > closeCount) {
        logDebug("Adding missing closing parentheses", "CALC");
        addedClosingParenthesis = openCount - closeCount;
        for (int i = 0; i < addedClosingParenthesis; i++) {
            currentExpression += ")";
        }
        UpdateDisplay(currentExpression);
        
        std::string noteMsg = std::to_string(addedClosingParenthesis) + 
                             " closing parentheses were added to " + originalExpr;
        logDebug(noteMsg, "CALC");
        
        originalExpr = currentExpression;
    }
    
    debugMsg = "Starting evaluation of expression: " + originalExpr;
    logDebug(debugMsg, "CALC");
    
    double result;
    try {
        result = EvaluateExpression(currentExpression);
        std::string resultMsg = "Result: " + std::to_string(result) + " (from expression: " + originalExpr + ")";
        logDebug(resultMsg, "CALC");
        
        std::string displayExpression = originalExpr;
        size_t pos = 0;
        while ((pos = displayExpression.find('*', pos)) != std::string::npos) {
            displayExpression.replace(pos, 1, "x");
        }
        
        std::string resultStr = std::to_string(result);
        resultStr.erase(resultStr.find_last_not_of('0') + 1, std::string::npos);
        if (resultStr.back() == '.') {
            resultStr.pop_back();
        }
        
        std::string historyEntry = displayExpression + " = " + resultStr;
        calculationHistory.push_back(historyEntry);
        
        if (calculationHistory.size() > 20) {
            calculationHistory.erase(calculationHistory.begin());
        }
        
        UpdateHistoryDisplay();
        
        currentExpression = resultStr;
        
        logDebug("Updating display with result", "CALC");
        UpdateDisplay(currentExpression);
        newExpression = true;
        
    } catch (const std::exception& e) {
        std::string errorMsg = "Error during calculation: " + std::string(e.what());
        logDebug(errorMsg, "ERROR");
        
        calculationHistory.push_back(originalExpr + " = Error: " + e.what());
        
        UpdateHistoryDisplay();
        
        std::string errorStr = e.what();
        if (errorStr.find("Function") != std::string::npos && errorStr.find("requires an argument") != std::string::npos) {
            MessageBoxW(hWndMain, L"Function requires an argument. Please add a value inside the parentheses.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        } 
        else if (errorStr.find("must be between") != std::string::npos) {
            MessageBoxW(hWndMain, L"Function argument is out of valid range. Please check the input value.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        else if (errorStr.find("Unknown identifier") != std::string::npos) {
            MessageBoxW(hWndMain, L"Unknown identifier found. Please check for typos or invalid characters.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        else if (errorStr.find("Division by zero") != std::string::npos) {
            MessageBoxW(hWndMain, L"Cannot divide by zero.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        else {
            std::wstring wideErrorMsg = StringToWString("Error: " + errorStr);
            MessageBoxW(hWndMain, wideErrorMsg.c_str(), L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        }
        
        currentExpression = originalExpr;
        UpdateDisplay(currentExpression);
        newExpression = false;
    } catch (...) {
        std::string errorMsg = "Unknown error during calculation";
        logDebug(errorMsg, "ERROR");
        
        calculationHistory.push_back(originalExpr + " = Error: Unknown error");
        
        UpdateHistoryDisplay();
        
        MessageBoxW(hWndMain, L"An unknown error occurred during calculation.", L"Calculation Error", MB_OK | MB_ICONINFORMATION);
        currentExpression = originalExpr;
        UpdateDisplay(currentExpression);
        newExpression = false;
    }
}

void ClearCalculator() {
    currentExpression = "0";
    newExpression = true;
    UpdateDisplay(currentExpression);
}

void ShowAboutDialog(HWND hwnd) {
    logDebug("About dialog shown", "UI");
    
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
        L"┬⌐ 2024 - MIT License",
        L"About C++ Calculator",
        MB_OK | MB_ICONINFORMATION);
}

bool IsOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == 'x' || c == '/' || c == '^' || c == '%';
}

int GetPrecedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 3;
    if (op == '%')
        return 2;  // Give % same precedence as multiplication
    return 0;  // Default case for any other operator
}

double ApplyOperator(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': 
            return a * b;
        case '/': 
            if (b == 0) throw std::runtime_error("Division by zero");
            return a / b;
        case '%':
            if (b == 0) throw std::runtime_error("Modulo by zero");
            return std::fmod(a, b);
        case '^': 
            try {
                return calculator.power(a, b);
            } catch (const std::exception& e) {
                throw std::runtime_error(std::string("Power error: ") + e.what());
            }
        default: throw std::runtime_error("Unknown operator: " + std::string(1, op));
    }
}

bool IsFunction(const std::string& token) {
    static const std::vector<std::string> functions = {
        "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "sqrt", "log", "ln",
        "abs", "fact"
    };
    
    return std::find(functions.begin(), functions.end(), token) != functions.end();
}

bool IsConstant(const std::string& token) {
    return token == "pi" || token == "e";
}

double GetConstantValue(const std::string& token) {
    if (token == "pi") return calculator.getPi();
    if (token == "e") return calculator.getE();
    throw std::invalid_argument("Unknown constant: " + token);
}

double ParseFunction(const std::string& func, const std::string& expr, size_t& i) {
    logDebug("ParseFunction called with function: " + func + ", expr: " + expr + ", i: " + std::to_string(i), "CALC");
    
    size_t funcPos = expr.find(func, i > func.length() ? i - func.length() : 0);
    if (funcPos == std::string::npos) {
        funcPos = expr.find(func);
    }
    
    if (funcPos == std::string::npos) {
        std::string errorMsg = "Function " + func + " not found in expression " + expr;
        logDebug(errorMsg, "ERROR");
        throw std::runtime_error("Function not found in expression");
    }
    
    size_t openParenPos = funcPos + func.length();
    
    if (openParenPos >= expr.length() || expr[openParenPos] != '(') {
        std::string errorMsg = "Missing opening parenthesis after function " + func;
        logDebug(errorMsg, "ERROR");
        
        if (openParenPos < expr.length() && 
            (isdigit(expr[openParenPos]) || expr[openParenPos] == '.' || 
             expr[openParenPos] == 'p' || expr[openParenPos] == 'e')) {
            
            std::string correctedExpr = expr.substr(0, openParenPos) + "(" + expr.substr(openParenPos);
            
            size_t closePos = openParenPos + 1;
            while (closePos < correctedExpr.length() && 
                  (isdigit(correctedExpr[closePos]) || correctedExpr[closePos] == '.' || 
                   correctedExpr[closePos] == 'e' || correctedExpr[closePos] == 'E' ||
                   correctedExpr[closePos] == 'p' || correctedExpr[closePos] == 'i')) {
                closePos++;
            }
            
            correctedExpr.insert(closePos, ")");
            
            logDebug("Auto-corrected function call: " + correctedExpr, "CALC");
            
            return EvaluateExpression(correctedExpr);
        }
        
        throw std::runtime_error("Missing opening parenthesis after function");
    }
    
    size_t start = openParenPos + 1;
    size_t pos = start;
    
    std::string debugParens = "Parsing function arguments: Start position = " + std::to_string(start) + 
                            ", Expression length = " + std::to_string(expr.length());
    logDebug(debugParens, "CALC");
    
    int parenCount = 1;
    while (pos < expr.length() && parenCount > 0) {
        if (expr[pos] == '(') parenCount++;
        if (expr[pos] == ')') parenCount--;
        pos++;
    }
    
    if (parenCount > 0) {
        std::string errorMsg = "Missing closing parenthesis for function " + func;
        logDebug(errorMsg, "ERROR");
        throw std::runtime_error("Missing closing parenthesis for function");
    }
    
    std::string argStr = expr.substr(start, pos - start - 1);
    
    std::string debugArgStr = "Extracted argument: " + argStr;
    logDebug(debugArgStr, "CALC");
    
    if (argStr.empty()) {
        std::string errorMsg = "Function " + func + " requires an argument";
        logDebug(errorMsg, "ERROR");
        
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
    
    i = pos;
    
    double argValue = EvaluateExpression(argStr);
    
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

double EvaluateExpression(const std::string& expression) {
    std::string debugMsg = "EvaluateExpression called with: " + expression;
    logDebug(debugMsg, "CALC");
    
    std::vector<std::string> functions = {"sin", "cos", "tan", "asin", "acos", "atan", 
                                        "sinh", "cosh", "tanh", "sqrt", "log", "ln",
                                        "abs", "fact"};
    
    std::string processedExpr = expression;
    
    // Special handling for abs with negative numbers: abs(-X) needs special care
    size_t absPos = 0;
    while ((absPos = processedExpr.find("abs(", absPos)) != std::string::npos) {
        size_t argStart = absPos + 4; // position after "abs("
        if (argStart < processedExpr.length() && processedExpr[argStart] == '-') {
            // We have abs(-something) - which is a unary minus inside abs
            // Ensure we have proper expression for this case
            size_t rightParenPos = argStart;
            int parenCount = 1;
            while (rightParenPos < processedExpr.length() && parenCount > 0) {
                rightParenPos++;
                if (rightParenPos < processedExpr.length()) {
                    if (processedExpr[rightParenPos] == '(') parenCount++;
                    if (processedExpr[rightParenPos] == ')') parenCount--;
                }
            }
            
            // If we found the closing parenthesis, handle the abs(-X) case specially
            if (parenCount == 0 && rightParenPos < processedExpr.length()) {
                std::string argContent = processedExpr.substr(argStart, rightParenPos - argStart);
                if (argContent.length() > 0 && argContent[0] == '-') {
                    // Replace abs(-X) with abs(0-X) which parser can handle better
                    processedExpr = processedExpr.substr(0, argStart) + "0" + 
                                   processedExpr.substr(argStart);
                }
            }
        }
        absPos += 4; // Move past "abs("
    }
    
    for (const std::string& func : functions) {
        size_t pos = 0;
        while ((pos = processedExpr.find(func, pos)) != std::string::npos) {
            bool isValidFunction = false;
            
            if (pos == 0 || IsOperator(processedExpr[pos-1]) || processedExpr[pos-1] == '(' || 
                processedExpr[pos-1] == '*' || processedExpr[pos-1] == '/' || 
                processedExpr[pos-1] == '+' || processedExpr[pos-1] == '-') {
                isValidFunction = true;
            }
            
            if (isValidFunction) {
                size_t nextPos = pos + func.length();
                if (nextPos < processedExpr.length() && processedExpr[nextPos] != '(') {
                    if (nextPos < processedExpr.length() && 
                        (isdigit(processedExpr[nextPos]) || processedExpr[nextPos] == '.' || 
                         processedExpr[nextPos] == 'p' || processedExpr[nextPos] == 'e')) {
                        
                        processedExpr.insert(nextPos, "(");
                        
                        size_t argEnd = nextPos + 1;
                        int nestedParens = 0;
                        
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
                        
                        if (argEnd == processedExpr.length()) {
                            processedExpr += ")";
                        } else if (!IsOperator(processedExpr[argEnd]) && processedExpr[argEnd] != ')') {
                            processedExpr.insert(argEnd + 1, ")");
                        } else {
                            processedExpr.insert(argEnd, ")");
                        }
                        
                        std::string fixedExpr = "Auto-fixed function '" + func + "' without parentheses: " + processedExpr;
                        logDebug(fixedExpr, "CALC");
                    }
                }
            }
            
            pos += func.length();
        }
    }
    
    if (processedExpr != expression) {
        logDebug("Using modified expression: " + processedExpr, "CALC");
        return EvaluateExpression(processedExpr);
    }
    
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
        if (expression[i] == ' ')
            continue;
        
        if (expression[i] == '-' && expectingOperand) {
            logDebug("Found unary minus", "CALC");
            operators.push('-');
            expectingOperand = true;
            continue;
        }
        
        if (expression[i] == '+' && expectingOperand) {
            logDebug("Found unary plus, ignoring", "CALC");
        }
        
        if (expression[i] == '(') {
            operators.push(expression[i]);
            expectingOperand = true;
            continue;
        }
        
        if (expression[i] == ')') {
            if (expectingOperand) {
                throw std::runtime_error("Invalid expression: empty parentheses or missing operand before ')'");
            }
            
            bool foundMatchingParen = false;
            
            while (!operators.empty()) {
                if (operators.top() == '(') {
                    foundMatchingParen = true;
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
        
        if (isalpha(expression[i])) {
            currentToken.clear();
            
            while (i < expression.length() && (isalpha(expression[i]) || isdigit(expression[i]))) {
                currentToken += expression[i++];
            }
            
            if (IsConstant(currentToken)) {
                values.push(GetConstantValue(currentToken));
                expectingOperand = false;
                i--; // Adjust index since we incremented it in the while loop
                continue;
            }
            
            if (IsFunction(currentToken) && i < expression.length() && expression[i] == '(') {
                try {
                    double functionResult = ParseFunction(currentToken, expression, i);
                    values.push(functionResult);
                    expectingOperand = false;
                    continue;
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string(e.what()));
                }
            }
            
            throw std::runtime_error("Unknown identifier: '" + currentToken + "'");
        }
        
        if (IsOperator(expression[i])) {
            if (expectingOperand) {
                throw std::runtime_error("Invalid expression: operator '" + std::string(1, expression[i]) + 
                                         "' cannot follow another operator");
            }
            
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
            
            operators.push(expression[i]);
            expectingOperand = true;
            continue;
        }
        
        if (isdigit(expression[i]) || expression[i] == '.') {
            std::string currentNumber;
            
            while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
                currentNumber += expression[i++];
            }
            
            if (std::count(currentNumber.begin(), currentNumber.end(), '.') > 1) {
                throw std::runtime_error("Invalid number format: multiple decimal points in '" + currentNumber + "'");
            }
            
            try {
                values.push(std::stod(currentNumber));
            } catch (...) {
                throw std::runtime_error("Invalid number format: '" + currentNumber + "'");
            }
            
            expectingOperand = false;
            i--; // Adjust index since we incremented it in the while loop
            continue;
        }
        
        throw std::runtime_error("Unrecognized character in expression: '" + std::string(1, expression[i]) + "'");
    }
    
    if (expectingOperand && !operators.empty()) {
        throw std::runtime_error("Invalid expression: ends with an operator");
    }
    
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

void SwitchTheme() {
    currentThemeIndex = (currentThemeIndex + 1) % availableThemes.size();
    currentTheme = &availableThemes[currentThemeIndex];
    
    if (themeBackgroundBrush != NULL) {
        DeleteObject(themeBackgroundBrush);
        themeBackgroundBrush = NULL;
    }
    
    for (int i = 0; i < 10; i++) {
        if (buttonBrushes[i] != NULL) {
            DeleteObject(buttonBrushes[i]);
            buttonBrushes[i] = NULL;
        }
    }
    
    themeBackgroundBrush = CreateSolidBrush(currentTheme->windowBackground);
    
    std::string themeName = WStringToString(currentTheme->name);
    logDebug("Theme changed to: " + themeName, "UI");
}

void ApplyTheme(HWND hwnd) {
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)themeBackgroundBrush);
    
    SendMessage(hWndDisplay, EM_SETBKGNDCOLOR, 0, currentTheme->displayBackground);
    
    SendMessage(hWndHistoryList, EM_SETBKGNDCOLOR, 0, currentTheme->historyBackground);
    
    for (int i = 0; i < 45; i++) {
        if (hWndButtons[i] != NULL) {
            InvalidateRect(hWndButtons[i], NULL, TRUE);
        }
    }
    
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    
    UpdateMemoryIndicator();
    UpdateHistoryDisplay();
}

void CleanupResources() {
    if (themeBackgroundBrush != NULL) {
        DeleteObject(themeBackgroundBrush);
        themeBackgroundBrush = NULL;
    }

    for (int i = 0; i < 10; i++) {
        if (buttonBrushes[i] != NULL) {
            DeleteObject(buttonBrushes[i]);
            buttonBrushes[i] = NULL;
        }
    }

    if (hDisplayFont != NULL) {
        DeleteObject(hDisplayFont);
        hDisplayFont = NULL;
    }

    if (hHistoryFont != NULL) {
        DeleteObject(hHistoryFont);
        hHistoryFont = NULL;
    }

    for (int i = 0; i < 45; i++) {
        if (hButtonFonts[i] != NULL) {
            DeleteObject(hButtonFonts[i]);
            hButtonFonts[i] = NULL;
        }
    }
}

bool IsValidIdentifier(const std::string& token) {
    if (IsFunction(token)) return true;
    
    if (IsConstant(token)) return true;
    
    if (!token.empty() && (isdigit(token[0]) || token[0] == '.')) {
        bool hasDecimal = false;
        for (size_t i = 0; i < token.length(); i++) {
            if (token[i] == '.') {
                hasDecimal = true;
            } else if (!isdigit(token[i])) {
            }
        }
        return true;
    }
    
    return false;
}

std::string GetLastToken(const std::string& expr) {
    if (expr.empty()) return "";
    
    size_t lastNonAlphaNumPos = expr.find_last_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.");
    
    if (lastNonAlphaNumPos == std::string::npos) {
    } else if (lastNonAlphaNumPos == expr.length() - 1) {
    }
    
    return expr.substr(lastNonAlphaNumPos + 1);
}

std::string FormatExpressionWithPrecedence(const std::string& expr) {
    std::string result = expr;
    
    size_t pos = 0;
    while ((pos = result.find('*', pos)) != std::string::npos) {
        result.replace(pos, 1, "x");
        pos += 1;
    }
    
    std::vector<std::pair<size_t, char>> operators;
    for (size_t i = 0; i < result.length(); i++) {
        char currentChar = result[i];
        if (currentChar == '+' || currentChar == '-' || 
            currentChar == 'x' || currentChar == '/' || 
            currentChar == '^' || currentChar == '%') {
            operators.push_back({i, currentChar});
        }
    }
    
    for (auto it = operators.rbegin(); it != operators.rend(); ++it) {
        size_t pos = it->first;
        char op = it->second;
        
        if (pos == 0 || result[pos-1] == '(' || 
            (pos > 0 && (result[pos-1] == '+' || result[pos-1] == '-' || 
                         result[pos-1] == 'x' || result[pos-1] == '/' || 
                         result[pos-1] == '^' || result[pos-1] == '%'))) {
            continue;
        }
        
        int precedence = 0;
        if (op == '+' || op == '-') precedence = 1;
        else if (op == 'x' || op == '/' || op == '%') precedence = 2;
        else if (op == '^') precedence = 3;
        
        std::string replacement;
        switch (precedence) {
            default: replacement = std::string(1, op); break;
        }
        
        result.replace(pos, 1, replacement);
        
        for (auto jt = it.base(); jt != operators.end(); ++jt) {
            if (jt->first > pos) {
                jt->first += replacement.length() - 1;
            }
        }
    }
    
    return result;
}

@echo off
echo Compiling C++ Calculator with g++...

REM Compile resource file
windres calculator.rc -O coff -o calculator.res

REM Compile GUI version
g++ -std=c++11 -mwindows main.cpp calculator.cpp calculator.res -o calculator.exe

echo.
echo Compilation completed!
echo.
echo To run the calculator, type: start calculator.exe
echo Or run the run.bat script
echo. 
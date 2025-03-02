@echo off
echo Starting C++ Calculator...

if exist calculator.exe (
    start calculator.exe
) else (
    echo Error: Calculator executable not found.
    echo Please compile the project first.
    pause
) 
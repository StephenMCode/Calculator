cmake_minimum_required(VERSION 3.10)
project(Calculator VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
add_executable(calculator main.cpp calculator.cpp)

# Install target
install(TARGETS calculator DESTINATION bin) 
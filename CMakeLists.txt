# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name and programming language
project(TicTacToe C)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Add the executable
add_executable(main main.c)

# Include directories for raylib
include_directories(C:/raylib/w64devkit/x86_64-w64-mingw32/include)

# Link directories for raylib
link_directories(C:/raylib/w64devkit/x86_64-w64-mingw32/lib)

# Link raylib and system libraries (OpenGL, GDI, Winmm)
target_link_libraries(main raylib opengl32 gdi32 winmm)

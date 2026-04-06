@echo off
if exist build (
    echo [INFO] Cleaning old build directory...
    rmdir /s /q build
)
mkdir build
cd build

echo [INFO] Generating Visual Studio 2022 Solution...
cmake .. -G "Visual Studio 18 2026" -A x64

if %errorlevel% equ 0 (
    echo [SUCCESS] Project generation complete!
    start CS_Engine.sln
) else (
    echo [ERROR] CMake generation failed.
    pause
)
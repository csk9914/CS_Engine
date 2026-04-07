@echo off
setlocal
set PROJECT_NAME=CS_Engine

if exist build (
    echo [INFO] Cleaning old build directory...
    rmdir /s /q build
)

mkdir build
cd build

echo [INFO] Detecting and Generating Visual Studio Solution...
cmake .. -A x64

if %errorlevel% equ 0 (
    echo [SUCCESS] Project generation complete!
    if exist %PROJECT_NAME%.sln (
        echo [INFO] Opening %PROJECT_NAME%.sln...
        start %PROJECT_NAME%.sln
    )
) else (
    echo [ERROR] CMake generation failed.
    echo [TIP] Make sure "Desktop development with C++" is installed in VS 2026 Installer.
    pause
)
endlocal
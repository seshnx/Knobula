@echo off
REM Aetheri Clean Script (Batch version for Windows)
REM Removes all build artifacts and temporary files

setlocal enabledelayedexpansion

echo ========================================
echo Aetheri Clean Script
echo ========================================
echo.

REM Get script directory
set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%"
set "BUILD_DIR=%PROJECT_DIR%build"

echo Project Directory: %PROJECT_DIR%
echo.

REM Clean build directory
if exist "%BUILD_DIR%" (
    echo Removing build directory...
    rmdir /s /q "%BUILD_DIR%"
    echo Build directory removed.
) else (
    echo Build directory not found. Nothing to clean.
)

echo.
echo ========================================
echo Clean completed successfully!
echo ========================================
echo.
echo To rebuild, run: build.bat
echo.

endlocal


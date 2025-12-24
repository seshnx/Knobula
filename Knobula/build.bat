@echo off
REM Aetheri Build Script (Batch version for Windows)
REM Usage: build.bat [Release|Debug] [clean] [configure]

setlocal enabledelayedexpansion

set CONFIG=Release
set CLEAN=0
set CONFIGURE=0

REM Parse arguments
:parse_args
if "%~1"=="" goto :end_parse
if /i "%~1"=="clean" set CLEAN=1
if /i "%~1"=="configure" set CONFIGURE=1
if /i "%~1"=="Release" set CONFIG=Release
if /i "%~1"=="Debug" set CONFIG=Debug
shift
goto :parse_args
:end_parse

echo ========================================
echo Knobula Build Script
echo ========================================
echo Configuration: %CONFIG%
echo.

REM Get script directory
set "SCRIPT_DIR=%~dp0"
set "PROJECT_DIR=%SCRIPT_DIR%"
set "BUILD_DIR=%PROJECT_DIR%build"

REM Clean build directory if requested
if %CLEAN%==1 (
    if exist "%BUILD_DIR%" (
        echo Cleaning build directory...
        rmdir /s /q "%BUILD_DIR%"
        echo Build directory cleaned.
        echo.
    )
)

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    echo Creating build directory...
    mkdir "%BUILD_DIR%"
    echo Build directory created.
    echo.
    set CONFIGURE=1
)

REM Change to build directory
cd /d "%BUILD_DIR%"

REM Configure with CMake if needed
if %CONFIGURE%==1 (
    if not exist "CMakeCache.txt" (
        echo Configuring CMake...
        cmake .. -DCMAKE_BUILD_TYPE=%CONFIG%
        if errorlevel 1 (
            echo CMake configuration failed!
            exit /b 1
        )
        echo CMake configuration successful.
        echo.
    )
) else (
    if not exist "CMakeCache.txt" (
        echo Configuring CMake...
        cmake .. -DCMAKE_BUILD_TYPE=%CONFIG%
        if errorlevel 1 (
            echo CMake configuration failed!
            exit /b 1
        )
        echo CMake configuration successful.
        echo.
    ) else (
        echo Using existing CMake configuration.
        echo.
    )
)

REM Build the project
echo Building project...
echo.

cmake --build . --config %CONFIG%

if errorlevel 1 (
    echo.
    echo Build failed!
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.

REM Show output location
set "VST3_PATH=%BUILD_DIR%\Knobula_artefacts\%CONFIG%\VST3\Knobula.vst3"
set "AU_PATH=%BUILD_DIR%\Knobula_artefacts\%CONFIG%\AU\Knobula.component"
set "STANDALONE_PATH=%BUILD_DIR%\Knobula_artefacts\%CONFIG%\Standalone\Knobula.exe"

echo Output files:
if exist "%VST3_PATH%" (
    echo   VST3: %VST3_PATH%
)
if exist "%AU_PATH%" (
    echo   AU: %AU_PATH%
)
if exist "%STANDALONE_PATH%" (
    echo   Standalone: %STANDALONE_PATH%
)
echo.

endlocal


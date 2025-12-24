#!/bin/bash
# Aetheri Build Script for macOS
# Usage: ./build.sh [Release|Debug] [--clean] [--configure] [--universal]

set -e  # Exit on error

# Default values
CONFIG="Release"
CLEAN=false
CONFIGURE=false
UNIVERSAL=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        Release|Debug)
            CONFIG="$1"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --configure)
            CONFIGURE=true
            shift
            ;;
        --universal)
            UNIVERSAL=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [Release|Debug] [--clean] [--configure] [--universal]"
            exit 1
            ;;
    esac
done

# Get script directory (project root)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build"

echo "========================================"
echo "Aetheri Build Script (macOS)"
echo "========================================"
echo "Project Directory: $PROJECT_DIR"
echo "Build Directory: $BUILD_DIR"
echo "Configuration: $CONFIG"
if [ "$UNIVERSAL" = true ]; then
    echo "Architecture: Universal (arm64 + x86_64)"
else
    echo "Architecture: Native"
fi
echo ""

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Warning: This script is designed for macOS. Continuing anyway..." >&2
fi

# Check for required tools
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is not installed. Please install CMake 3.22 or higher." >&2
    exit 1
fi

if ! command -v clang++ &> /dev/null; then
    echo "Error: clang++ is not installed. Please install Xcode Command Line Tools." >&2
    exit 1
fi

# Clean build directory if requested
if [ "$CLEAN" = true ] && [ -d "$BUILD_DIR" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Build directory cleaned."
    echo ""
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
    echo "Build directory created."
    echo ""
    CONFIGURE=true
fi

# Change to build directory
cd "$BUILD_DIR"

# Configure with CMake if needed
if [ "$CONFIGURE" = true ] || [ ! -f "CMakeCache.txt" ]; then
    echo "Configuring CMake..."
    
    CMAKE_ARGS=(
        ".."
        "-DCMAKE_BUILD_TYPE=$CONFIG"
    )
    
    # Add universal build option if requested
    if [ "$UNIVERSAL" = true ]; then
        CMAKE_ARGS+=("-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64")
    fi
    
    cmake "${CMAKE_ARGS[@]}"
    
    if [ $? -ne 0 ]; then
        echo "CMake configuration failed!" >&2
        exit 1
    fi
    
    echo "CMake configuration successful."
    echo ""
else
    echo "Using existing CMake configuration."
    echo ""
fi

# Build the project
echo "Building project..."
echo ""

# Use all available CPU cores for building
CPU_COUNT=$(sysctl -n hw.ncpu 2>/dev/null || echo "4")
cmake --build . --config "$CONFIG" -j "$CPU_COUNT"

if [ $? -ne 0 ]; then
    echo ""
    echo "Build failed!" >&2
    exit 1
fi

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""

# Show output locations
ARTEFACTS_DIR="$BUILD_DIR/Aetheri_artefacts/$CONFIG"
VST3_PATH="$ARTEFACTS_DIR/VST3/Aetheri.vst3"
AU_PATH="$ARTEFACTS_DIR/AU/Aetheri.component"
AAX_PATH="$ARTEFACTS_DIR/AAX/Aetheri.aaxplugin"
STANDALONE_PATH="$ARTEFACTS_DIR/Standalone/Aetheri.app"

echo "Output files:"
if [ -d "$VST3_PATH" ]; then
    echo "  VST3: $VST3_PATH"
fi
if [ -d "$AU_PATH" ]; then
    echo "  AU: $AU_PATH"
fi
if [ -d "$AAX_PATH" ]; then
    echo "  AAX: $AAX_PATH"
fi
if [ -d "$STANDALONE_PATH" ]; then
    echo "  Standalone: $STANDALONE_PATH"
fi
echo ""

# Installation instructions
echo "To install plugins:"
echo "  VST3: Copy to ~/Library/Audio/Plug-Ins/VST3/"
echo "  AU:   Copy to ~/Library/Audio/Plug-Ins/Components/"
echo "  AAX:  Copy to /Library/Application Support/Avid/Audio/Plug-Ins/"
echo ""


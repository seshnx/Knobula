#!/bin/bash
# Aetheri Clean Script for macOS/Linux
# Removes all build artifacts and temporary files

set -e  # Exit on error

# Default values
CLEAN_DEPS=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --all)
            CLEAN_DEPS=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--all]"
            echo "  --all: Also remove JUCE dependencies (will re-download on next build)"
            exit 1
            ;;
    esac
done

# Get script directory (project root)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$SCRIPT_DIR"

echo "========================================"
echo "Aetheri Clean Script"
echo "========================================"
echo "Project Directory: $PROJECT_DIR"
echo ""

# Directories to clean
BUILD_DIR="$PROJECT_DIR/build"
DEPS_DIR="$BUILD_DIR/_deps"

# Clean build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Removing build directory..."
    rm -rf "$BUILD_DIR"
    echo "Build directory removed."
else
    echo "Build directory not found. Nothing to clean."
fi

# Clean any other temporary files
TEMP_PATTERNS=(
    "*.user"
    "*.suo"
    "*.sdf"
    "*.opensdf"
    "*.db"
    "*.opendb"
    ".vs"
    ".idea"
    "*.swp"
    "*.swo"
    "*~"
    ".DS_Store"
)

CLEANED_COUNT=0
for pattern in "${TEMP_PATTERNS[@]}"; do
    find "$PROJECT_DIR" -name "$pattern" -type f -delete 2>/dev/null || true
    CLEANED_COUNT=$((CLEANED_COUNT + $(find "$PROJECT_DIR" -name "$pattern" -type f 2>/dev/null | wc -l)))
done

if [ $CLEANED_COUNT -gt 0 ]; then
    echo "Removed temporary files."
fi

echo ""
echo "========================================"
echo "Clean completed successfully!"
echo "========================================"
echo ""
echo "To rebuild, run: ./build.sh"
echo ""


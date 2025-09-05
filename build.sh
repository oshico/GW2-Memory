#!/bin/bash

# Nexus Addon Template Build Script
# Cleans and builds the addon using MinGW-w64 cross-compilation

set -e  # Exit on any error

echo "=== Nexus Addon Template Build Script ==="

# Check if MinGW-w64 is installed
if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    echo "Error: x86_64-w64-mingw32-gcc not found. Please install MinGW-w64:"
    echo "  sudo apt install mingw-w64 cmake make"
    exit 1
fi

# Clean previous build
echo "Cleaning previous build..."
if [ -d "build" ]; then
    rm -rf build
    echo "Removed existing build directory"
fi

# Create fresh build directory
echo "Creating build directory..."
mkdir build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres \
    -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building addon..."
make -j$(nproc)

# Check if build was successful
if [ -f "bin/Memory.dll" ]; then
    echo ""
    echo "=== BUILD SUCCESSFUL ==="
    echo "Output: $(pwd)/bin/Memory.dll"
    echo "Size: $(du -h bin/Memory.dll | cut -f1)"
    exit 0
else
    echo ""
    echo "=== BUILD FAILED ==="
    echo "DLL not found in expected location"
    exit 1
fi
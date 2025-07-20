#!/bin/bash

# Build script for Prompt Manager Desktop

set -e

BUILD_DIR="build"
SOURCE_DIR="$(pwd)"

echo "Building Prompt Manager Desktop..."

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure with CMake
echo "Configuring with CMake..."
cmake "$SOURCE_DIR" -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo "Executable: $BUILD_DIR/PromptManagerDesktop"

# Make executable
chmod +x PromptManagerDesktop

echo ""
echo "To run the application:"
echo "  cd $BUILD_DIR && ./PromptManagerDesktop"
#!/bin/bash

# Build script for STL Viewer

set -e

echo "Building STL Viewer..."

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building..."
make -j$(nproc)

echo "Build completed successfully!"
echo "To run the application, use: ./build/STLViewer"
echo "Or from the build directory: ./STLViewer"

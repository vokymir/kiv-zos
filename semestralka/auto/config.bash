#!/bin/bash

mkdir -p build
mkdir -p bin

# Set the C and C++ compilers to Clang (for consistency)
export CC=clang
export CXX=clang++

# Run the CMake configuration command
echo "Running CMake configuration with Clang and Debug type..."
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

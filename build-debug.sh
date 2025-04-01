#!/bin/sh

# Check if a build directory exists
if [ -d "build" ]; then
  # Remove the old build directory
  echo "Removing old build directory..."
  rm -rf build
fi

# Create a new build directory
mkdir build

# Change to the build directory
cd build

# Run CMake to generate the build files
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
cmake --build .

# Return to the original directory
cd ..

./build/LabHash

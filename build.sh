#!/bin/bash

# Exit on error
set -e

# Check if the libraries exists, if not, run fetch_libraries.sh
if [ ! -d "libs" ] || [ ! -d "libs/FreeRTOS" ] || [ ! -d "libs/CMSIS" ] || [ ! -d "libs/STM32F4xx_HAL_Driver" ]; then
    echo "Libraries not found. Running fetch_libraries.sh..."
    ./fetch_libraries.sh
fi

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
echo "Building the project..."
make -j$(sysctl -n hw.ncpu) VERBOSE=1

# Print binary size information
echo "Binary size information:"
arm-none-eabi-size STM32F446RE_ADC.elf

echo ""
echo "Build complete!"
echo "Output files:"
echo "- build/STM32F446RE_ADC.elf (ELF file for debugging with GDB)"
echo "- build/STM32F446RE_ADC.hex (HEX file for flashing)"
echo "- build/STM32F446RE_ADC.bin (Binary file for flashing)"

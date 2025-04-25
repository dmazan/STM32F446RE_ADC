#!/bin/bash

# Exit on error
set -e

# Create a libs directory if it doesn't exist
mkdir -p libs
cd libs

echo "Fetching FreeRTOS..."
if [ ! -d "FreeRTOS" ]; then
    # Clone FreeRTOS Kernel directly
    git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
    cd FreeRTOS
    git checkout V10.5.1  # Using a stable version
    cd ..
fi

echo "Fetching CMSIS..."
if [ ! -d "CMSIS" ]; then
    git clone https://github.com/ARM-software/CMSIS_5.git CMSIS
    cd CMSIS
    git checkout 5.9.0  # Using a stable version
    cd ..
fi

echo "Fetching stm32-ssd1306 from truncated repository http://github.com/dmazan/stm32-ssd1306..."
if [ ! -d "stm32-ssd1306" ]; then
    git clone https://github.com/dmazan/stm32-ssd1306.git stm32-ssd1306
    cd stm32-ssd1306
    git checkout dmazan-local-use
    cd ..
fi

echo "Fetching STM32F4 HAL..."
if [ ! -d "STM32F4xx_HAL_Driver" ]; then
    echo "Creating temp directory..."
    # Create temporary directory for the cube download
    mkdir -p temp_cube
    cd temp_cube
    
    echo "Downloading STM32CubeF4..."
    # Download the STM32CubeF4 package
    git clone https://github.com/STMicroelectronics/STM32CubeF4.git
    cd STM32CubeF4
    git checkout v1.28.0  # Using a stable version
    git submodule init
    git submodule update
    
    echo "Copying HAL drivers..."
    # Create target directories
    mkdir -p ../../STM32F4xx_HAL_Driver/Inc
    mkdir -p ../../STM32F4xx_HAL_Driver/Src
    mkdir -p ../../CMSIS/Device/ST/STM32F4xx/Include
    mkdir -p ../../CMSIS/Device/ST/STM32F4xx/Source
    
    # Copy HAL drivers
    cp -r Drivers/STM32F4xx_HAL_Driver/* ../../STM32F4xx_HAL_Driver/
    
    # Copy CMSIS device files
    cp -r Drivers/CMSIS/Device/ST/STM32F4xx/* ../../CMSIS/Device/ST/STM32F4xx/
    
    # Copy template files from STM32F446ZE-Nucleo
    cp Projects/STM32F446ZE-Nucleo/Templates/Inc/stm32f4xx_hal_conf.h ../../STM32F4xx_HAL_Driver/Inc/
    cp Projects/STM32F446ZE-Nucleo/Templates/Inc/stm32f4xx_it.h ../../STM32F4xx_HAL_Driver/Inc/
    cp Projects/STM32F446ZE-Nucleo/Templates/Src/stm32f4xx_it.c ../../STM32F4xx_HAL_Driver/Src/
    
    echo "Cleaning up..."
    cd ../.. 
    rm -rf temp_cube
fi

# Verify critical files exist
echo "Verifying library files..."
CRITICAL_FILES=(
    "STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"
    "STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_conf.h"
    "CMSIS/Device/ST/STM32F4xx/Include/stm32f446xx.h"
    "FreeRTOS/include/FreeRTOS.h"
)

for file in "${CRITICAL_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "Error: Critical file $file is missing!"
        exit 1
    fi
done

echo "All libraries have been downloaded and verified successfully!"

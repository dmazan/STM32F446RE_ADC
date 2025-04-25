# STM32F446RE_ADC

Multi-ADC simultaneous sampling implementation for STM32F744RE microcontroller using FreeRTOS.
(c) 2025 Duffy Mazan (dmazan@mazan.org)

## Features

- Triple ADC simultaneous sampling (ADC1, ADC2, ADC3) with DMA
- Conversion based on TIM2 scaled to 1000 Hz with 12-bit resolution
- DAC output generation (0-4095, incrementing by 100 every second)
- Real-time display of values on:
  - UART (115200 baud)
  - SSD1306 OLED display

## Hardware Configuration

- ADC Inputs:
  - ADC1: PA0
  - ADC2: PA1
  - ADC3: PA2
- DAC Output:
  - DAC Channel 2: PA5
- UART:
  - USART1 TX: PA9
  - USART1 RX: PA10
- I2C (OLED Display):
  - I2C1 SCL: PB6
  - I2C1 SDA: PB7

## Software Architecture

The project uses FreeRTOS for task management and includes:
- ADC Task: Handles ADC sampling and DMA transfers
- DAC Task: Generates incrementing output signal
- Report Task: Updates UART and OLED display with current values

Communication between tasks is handled through FreeRTOS queues and mutex protection for shared resources.

## Build Instructions

### Prerequisites

- CMake (version 3.15 or higher)
- ARM GCC Toolchain (arm-none-eabi-gcc)
- Make or Ninja build system

### Getting the Dependencies

Run the provided script to fetch all required libraries:

```bash
./fetch_libraries.sh
```

### Building the Project

#### Option 1: Using the build script (Recommended)

Simply run the build script which will handle everything for you:

```bash
./build.sh
```

This will:
- Create a build directory
- Configure the project with CMake
- Build the project
- Generate .elf (for debugging), .hex, and .bin files

#### Option 2: Manual build

1. Create a build directory:
```bash
mkdir build && cd build
```

2. Configure with CMake:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

3. Build the project:
```bash
make -j4
```

4. The output files will be created in the build directory:
   - `STM32F446RE_ADC.elf`: ELF file for debugging
   - `STM32F446RE_ADC.hex`: HEX file for flashing
   - `STM32F446RE_ADC.bin`: Binary file for flashing

#### Build Options

- Debug build (default):
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

- Release build (optimized for size):
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Flashing

You can flash the binary using ST-Link utility:
```bash
st-flash write STM32RE_ADC.bin 0x8000000
```

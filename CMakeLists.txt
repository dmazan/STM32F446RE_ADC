cmake_minimum_required(VERSION 3.15)

# Set toolchain file before project command
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi-gcc.cmake)

# Project name and language
project(STM32F446RE_ADC C ASM)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Enable assembler files preprocessing
enable_language(ASM)

# Compiler configurations
add_compile_options(
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -ffunction-sections
    -fdata-sections
    -Wall
    -fno-common
    -fstack-usage
)

# Define symbols for debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

# Add general defines
add_compile_definitions(
    USE_HAL_DRIVER
    STM32F446xx
    # Explicitly enable all HAL modules that are used
    HAL_MODULE_ENABLED
    HAL_ADC_MODULE_ENABLED
    HAL_DAC_MODULE_ENABLED
    HAL_DMA_MODULE_ENABLED
    HAL_GPIO_MODULE_ENABLED
    HAL_I2C_MODULE_ENABLED
    HAL_UART_MODULE_ENABLED
    HAL_RCC_MODULE_ENABLED
    HAL_TIM_MODULE_ENABLED
    HAL_CORTEX_MODULE_ENABLED
)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/CMSIS/CMSIS/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/portable/GCC/ARM_CM4F
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stm32-ssd1306/ssd1306
)

# Add linker options
add_link_options(
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -specs=nano.specs
    -lc
    -lm
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# Linker script
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F446RETx_FLASH.ld)
add_link_options(-T${LINKER_SCRIPT})

# Find all STM32 HAL driver source files
file(GLOB HAL_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/STM32F4xx_HAL_Driver/Src/*.c
)

# Exclude template files and any problematic files
list(FILTER HAL_SRC EXCLUDE REGEX ".*template.*\.c$")
list(FILTER HAL_SRC EXCLUDE REGEX ".*_it\.c$")

# FreeRTOS source files
set(FREERTOS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/portable/GCC/ARM_CM4F/port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/FreeRTOS/portable/MemMang/heap_4.c
)

# Application source files
set(APP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/adc_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dac_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/report_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hw_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stm32-ssd1306/ssd1306/ssd1306.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/stm32-ssd1306/ssd1306/ssd1306_fonts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32_hal_timebase_tim.c
)

# Assembly source files
file(GLOB ASM_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.s
)

# Create executable
add_executable(${PROJECT_NAME}
    ${APP_SRC}
    ${HAL_SRC}
    ${FREERTOS_SRC}
    ${ASM_SRC}
)

# Generate HEX and binary files and copy ELF file for debugging
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMENT "Building ${PROJECT_NAME}.elf, ${PROJECT_NAME}.hex and ${PROJECT_NAME}.bin"
)

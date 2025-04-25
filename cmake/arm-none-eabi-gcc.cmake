set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Search for ARM toolchain in the PATH
find_program(ARM_CC arm-none-eabi-gcc)
find_program(ARM_CXX arm-none-eabi-g++)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy)
find_program(ARM_SIZE arm-none-eabi-size)

# If toolchain not found in PATH, look for typical MacOS Homebrew installation
if(NOT ARM_CC)
    # Check Homebrew location for Apple Silicon Macs
    if(EXISTS "/opt/homebrew/bin/arm-none-eabi-gcc")
        set(ARM_CC "/opt/homebrew/bin/arm-none-eabi-gcc")
        set(ARM_CXX "/opt/homebrew/bin/arm-none-eabi-g++")
        set(ARM_OBJCOPY "/opt/homebrew/bin/arm-none-eabi-objcopy")
        set(ARM_SIZE "/opt/homebrew/bin/arm-none-eabi-size")
    # Check Homebrew location for Intel Macs
    elseif(EXISTS "/usr/local/bin/arm-none-eabi-gcc")
        set(ARM_CC "/usr/local/bin/arm-none-eabi-gcc")
        set(ARM_CXX "/usr/local/bin/arm-none-eabi-g++")
        set(ARM_OBJCOPY "/usr/local/bin/arm-none-eabi-objcopy")
        set(ARM_SIZE "/usr/local/bin/arm-none-eabi-size")
    else()
        message(FATAL_ERROR "ARM toolchain not found. Please install arm-none-eabi-gcc.")
    endif()
endif()

# Set toolchain paths
set(CMAKE_C_COMPILER ${ARM_CC})
set(CMAKE_CXX_COMPILER ${ARM_CXX})
set(CMAKE_OBJCOPY ${ARM_OBJCOPY})
set(CMAKE_SIZE ${ARM_SIZE})

# Don't run the linker on compiler check
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Where to look for the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "c compiler flags debug")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE INTERNAL "cxx compiler flags debug")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm compiler flags debug")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "linker flags debug")

set(CMAKE_C_FLAGS_RELEASE "-Os -g" CACHE INTERNAL "c compiler flags release")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g" CACHE INTERNAL "cxx compiler flags release")
set(CMAKE_ASM_FLAGS_RELEASE "-g" CACHE INTERNAL "asm compiler flags release")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "linker flags release")

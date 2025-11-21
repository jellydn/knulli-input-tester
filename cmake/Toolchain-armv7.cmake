# Cross-compilation toolchain for ARM32 (ARMv7)
# Target: Linux ARM32 (armv7l) - for backward compatibility with older ARM devices
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-armv7.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7l)

# Specify cross-compiler for ARM32
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++ CACHE FILEPATH "CXX compiler" FORCE)
set(CMAKE_AR arm-linux-gnueabihf-ar CACHE FILEPATH "AR tool" FORCE)
set(CMAKE_RANLIB arm-linux-gnueabihf-ranlib CACHE FILEPATH "RANLIB tool" FORCE)
set(CMAKE_STRIP arm-linux-gnueabihf-strip CACHE FILEPATH "STRIP tool" FORCE)
set(CMAKE_NM arm-linux-gnueabihf-nm CACHE FILEPATH "NM tool" FORCE)
set(CMAKE_OBJDUMP arm-linux-gnueabihf-objdump CACHE FILEPATH "OBJDUMP tool" FORCE)

# Compilation flags for ARM32
# -march=armv7-a: Target ARMv7-A architecture
# -mfpu=neon: Enable NEON SIMD support (available on most modern ARM boards)
# -mfloat-abi=hard: Use hardware floating-point ABI
set(CMAKE_C_FLAGS "-march=armv7-a -mfpu=neon -mfloat-abi=hard -fPIC" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "-march=armv7-a -mfpu=neon -mfloat-abi=hard -fPIC" CACHE STRING "CXX compiler flags" FORCE)

# Release build flags
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "C release flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "CXX release flags" FORCE)

# Debug build flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "C debug flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "CXX debug flags" FORCE)

# Don't try to run binaries compiled for the target system
set(CMAKE_CROSSCOMPILING TRUE CACHE BOOL "Cross-compiling" FORCE)

# Configure target environment for finding libraries and headers
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf CACHE PATH "Search prefix for target environment" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "Program search mode" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE STRING "Library search mode" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE STRING "Include search mode" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY CACHE STRING "Package search mode" FORCE)

# Linker flags for static linking
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc" CACHE STRING "Linker flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc" CACHE STRING "Shared linker flags" FORCE)

# Disable shared libraries for embedded deployment
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# Version information for cross-compilation
message(STATUS "Cross-compiling for Linux ARM32 (armv7l)")
message(STATUS "Toolchain: arm-linux-gnueabihf")
message(STATUS "FPU: NEON (hard float ABI)")

# Cross-compilation toolchain for ARM64 (aarch64)
# Target: Linux ARM64 (e.g., Knulli on TrimUI Smart Pro)
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-aarch64.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Specify cross-compiler
# Note: Uses aarch64-linux-gnu-* executables if available
# On macOS without proper toolchain, may need to use Docker or cross-compilation container
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++ CACHE FILEPATH "CXX compiler" FORCE)
set(CMAKE_AR aarch64-linux-gnu-ar CACHE FILEPATH "AR tool" FORCE)
set(CMAKE_RANLIB aarch64-linux-gnu-ranlib CACHE FILEPATH "RANLIB tool" FORCE)
set(CMAKE_STRIP aarch64-linux-gnu-strip CACHE FILEPATH "STRIP tool" FORCE)
set(CMAKE_NM aarch64-linux-gnu-nm CACHE FILEPATH "NM tool" FORCE)
set(CMAKE_OBJDUMP aarch64-linux-gnu-objdump CACHE FILEPATH "OBJDUMP tool" FORCE)

# Compilation flags for ARM64
# -march=armv8-a: Target ARMv8-A architecture (Cortex-A series, used in most modern ARM devices)
# -mtune=generic: Generic optimization for ARMv8-A
set(CMAKE_C_FLAGS "-march=armv8-a -mtune=generic -fPIC" CACHE STRING "C compiler flags" FORCE)
set(CMAKE_CXX_FLAGS "-march=armv8-a -mtune=generic -fPIC" CACHE STRING "CXX compiler flags" FORCE)

# Release build flags
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "C release flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "CXX release flags" FORCE)

# Debug build flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING "C debug flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "CXX debug flags" FORCE)

# Don't try to run binaries compiled for the target system
set(CMAKE_CROSSCOMPILING TRUE CACHE BOOL "Cross-compiling" FORCE)

# Configure target environment for finding libraries and headers
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu CACHE PATH "Search prefix for target environment" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "Program search mode" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE STRING "Library search mode" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE STRING "Include search mode" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY CACHE STRING "Package search mode" FORCE)

# Linker flags for static linking of STL and GCC runtime libraries (for portability)
# This ensures the binary doesn't depend on libstdc++.so.6 or libgcc_s.so.1 on target system
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc" CACHE STRING "Linker flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc" CACHE STRING "Shared linker flags" FORCE)

# Disable shared libraries for embedded deployment
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# Version information for cross-compilation
message(STATUS "Cross-compiling for Linux ARM64 (aarch64)")
message(STATUS "Toolchain: aarch64-linux-gnu")
message(STATUS "Target: Knulli/TrimUI Smart Pro")

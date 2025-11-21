cmake_minimum_required(VERSION 3.20)
project(KnulliInputTester CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add cmake modules (must be before any cross-compilation checks)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Include architecture verification module
include(check_architecture)

# Output directories with architecture-specific suffix for cross-compilation
if(CMAKE_CROSSCOMPILING)
    # When cross-compiling, create separate directories per target architecture
    set(ARCH_SUFFIX "-${CMAKE_SYSTEM_PROCESSOR}")
else()
    set(ARCH_SUFFIX "")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin${ARCH_SUFFIX})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${ARCH_SUFFIX})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib${ARCH_SUFFIX})

# Set default build type to Release for target platform
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "No build type specified, using Release (optimized for target device)")
endif()

# Find required packages - with fallback for development
find_package(Threads REQUIRED)

# UUID - required for device identification
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(UUID QUIET uuid)
    if(UUID_FOUND)
        # Use full library path from pkg-config
        set(UUID_LIBRARIES "${UUID_LDFLAGS}")
    endif()
endif()

if(NOT UUID_FOUND)
    # Try to find it manually (for macOS with homebrew)
    find_library(UUID_LIBRARY NAMES uuid ossp-uuid
        HINTS
        /opt/homebrew/Cellar/ossp-uuid/*/lib
        /usr/local/lib
        /usr/lib
    )
    find_path(UUID_INCLUDE_DIR NAMES uuid.h
        HINTS
        /opt/homebrew/Cellar/ossp-uuid/*/include/ossp
        /usr/local/include/ossp
        /usr/include/ossp
        /usr/include
    )
    
    if(UUID_LIBRARY AND UUID_INCLUDE_DIR)
        set(UUID_FOUND TRUE)
        set(UUID_LIBRARIES "${UUID_LIBRARY}")
        set(UUID_INCLUDE_DIRS "${UUID_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "uuid library not found. Install: brew install ossp-uuid (macOS) or apt-get install uuid-dev (Linux)")
    endif()
endif()

if(UUID_FOUND)
    message(STATUS "uuid found: ${UUID_LIBRARIES}")
endif()

# SDL2 - required for UI
# Force static linking for portable binary
set(SDL2_STATIC ON)
find_package(SDL2 QUIET)

# If SDL2 not found as module, try pkg-config
if(NOT SDL2_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2 QUIET sdl2)
    endif()
endif()

if(NOT SDL2_FOUND)
    message(WARNING "SDL2 not found. Install: brew install sdl2 (macOS) or apt-get install libsdl2-dev (Linux)")
    # Create stub for development
    set(SDL2_FOUND FALSE)
    set(SDL2_INCLUDE_DIRS "")
    set(SDL2_LIBRARIES "")
else()
    message(STATUS "SDL2 found: ${SDL2_LIBRARIES}")
    # Prefer static library if available
    if(EXISTS "/usr/lib/libSDL2.a" OR EXISTS "/usr/local/lib/libSDL2.a" OR EXISTS "/opt/homebrew/lib/libSDL2.a")
        message(STATUS "Static SDL2 library available - using static linking")
    else()
        message(WARNING "SDL2 appears to be dynamic-only - binary may require SDL2 on target system")
    endif()
endif()

# imgui - optional, required only for UI
# ImGui should be compiled from source for static linking
find_package(imgui QUIET)
if(NOT imgui_FOUND)
    message(WARNING "imgui not found. Install: brew install imgui (macOS) or apt-get install libimgui-dev (Linux)")
    # Create stub for development
    set(IMGUI_FOUND FALSE)
    set(IMGUI_INCLUDE_DIRS "")
    set(IMGUI_LIBRARIES "")
else()
    message(STATUS "imgui found: ${IMGUI_LIBRARIES}")
endif()

# libevdev - optional, required only for input
find_package(evdev QUIET)
if(NOT evdev_FOUND)
    message(WARNING "libevdev not found. Install: brew install libevdev (macOS) or apt-get install libevdev-dev (Linux)")
    set(EVDEV_FOUND FALSE)
    set(EVDEV_LIBRARIES "")
endif()

# Source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*/*.cpp
)

# Create main executable
add_executable(knulli-input-tester ${SOURCES})

target_include_directories(knulli-input-tester PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
)

target_link_libraries(knulli-input-tester
    ${SDL2_LIBRARIES}
    ${IMGUI_LIBRARIES}
    ${EVDEV_LIBRARIES}
    ${UUID_LIBRARIES}
    Threads::Threads
)

# Verify binary format (runs after successful build)
check_binary_format(knulli-input-tester)
if(CMAKE_CROSSCOMPILING)
    check_target_architecture("${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Compiler flags for optimization and warnings
if(MSVC)
    target_compile_options(knulli-input-tester PRIVATE /W4)
else()
    target_compile_options(knulli-input-tester PRIVATE -Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(knulli-input-tester PRIVATE -O3)
    endif()
endif()

# Testing setup
enable_testing()
find_package(GTest QUIET)
find_package(Catch2 QUIET)

if(NOT GTest_FOUND AND NOT GTEST_FOUND)
    message(WARNING "GTest not found. Unit tests will be skipped. Install: brew install googletest (macOS)")
endif()

if(NOT Catch2_FOUND AND NOT CATCH2_FOUND)
    message(WARNING "Catch2 not found. Integration/Contract tests will be skipped. Install: brew install catch2 (macOS)")
endif()

# Unit tests
file(GLOB_RECURSE TEST_SOURCES
    tests/unit/*.cpp
    tests/unit/*/*.cpp
)

if(TEST_SOURCES AND (GTest_FOUND OR GTEST_FOUND))
    add_executable(unit-tests ${TEST_SOURCES})
    target_include_directories(unit-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    if(GTest_FOUND)
        target_link_libraries(unit-tests
            GTest::GTest
            GTest::Main
            ${SDL2_LIBRARIES}
        )
    else()
        target_link_libraries(unit-tests
            ${GTEST_LIBRARIES}
            ${SDL2_LIBRARIES}
        )
        target_include_directories(unit-tests PRIVATE ${GTEST_INCLUDE_DIR})
    endif()
    add_test(NAME UnitTests COMMAND unit-tests)
elseif(TEST_SOURCES)
    message(STATUS "Unit tests available but GTest not found - skipping build")
endif()

# Integration tests
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES
    tests/integration/*.cpp
    tests/integration/*/*.cpp
)

if(INTEGRATION_TEST_SOURCES AND (Catch2_FOUND OR CATCH2_FOUND))
    add_executable(integration-tests ${INTEGRATION_TEST_SOURCES})
    target_include_directories(integration-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    if(Catch2_FOUND)
        target_link_libraries(integration-tests
            Catch2::Catch2
            Catch2::Catch2WithMain
            ${SDL2_LIBRARIES}
        )
    else()
        target_link_libraries(integration-tests
            ${SDL2_LIBRARIES}
        )
        target_include_directories(integration-tests PRIVATE ${CATCH2_INCLUDE_DIR})
    endif()
    add_test(NAME IntegrationTests COMMAND integration-tests)
elseif(INTEGRATION_TEST_SOURCES)
    message(STATUS "Integration tests available but Catch2 not found - skipping build")
endif()

# Contract tests
file(GLOB_RECURSE CONTRACT_TEST_SOURCES
    tests/contract/*.cpp
)

if(CONTRACT_TEST_SOURCES AND (Catch2_FOUND OR CATCH2_FOUND))
    add_executable(contract-tests ${CONTRACT_TEST_SOURCES})
    target_include_directories(contract-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    if(Catch2_FOUND)
        target_link_libraries(contract-tests
            Catch2::Catch2
            Catch2::Catch2WithMain
            ${SDL2_LIBRARIES}
        )
    else()
        target_link_libraries(contract-tests
            ${SDL2_LIBRARIES}
        )
        target_include_directories(contract-tests PRIVATE ${CATCH2_INCLUDE_DIR})
    endif()
    add_test(NAME ContractTests COMMAND contract-tests)
elseif(CONTRACT_TEST_SOURCES)
    message(STATUS "Contract tests available but Catch2 not found - skipping build")
endif()

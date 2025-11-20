cmake_minimum_required(VERSION 3.20)
project(KnulliInputTester CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find required packages - with fallback for development
find_package(Threads REQUIRED)

# SDL2 - required for UI
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    message(WARNING "SDL2 not found. Install: brew install sdl2 (macOS) or apt-get install libsdl2-dev (Linux)")
    # Create stub for development
    set(SDL2_FOUND FALSE)
    set(SDL2_INCLUDE_DIRS "")
    set(SDL2_LIBRARIES "")
endif()

# imgui - optional, required only for UI
find_package(imgui QUIET)
if(NOT imgui_FOUND)
    message(WARNING "imgui not found. Install: brew install imgui (macOS) or apt-get install libimgui-dev (Linux)")
    # Create stub for development
    set(IMGUI_FOUND FALSE)
    set(IMGUI_INCLUDE_DIRS "")
    set(IMGUI_LIBRARIES "")
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
)

target_link_libraries(knulli-input-tester
    ${SDL2_LIBRARIES}
    ${IMGUI_LIBRARIES}
    ${EVDEV_LIBRARIES}
    Threads::Threads
)

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

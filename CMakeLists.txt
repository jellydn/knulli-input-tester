cmake_minimum_required(VERSION 3.20)
project(KnulliInputTester CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(imgui REQUIRED)
find_package(evdev REQUIRED)
find_package(Threads REQUIRED)

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
    evdev
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
find_package(GTest REQUIRED)
find_package(Catch2 REQUIRED)

# Unit tests
file(GLOB_RECURSE TEST_SOURCES
    tests/unit/*.cpp
    tests/unit/*/*.cpp
)

if(TEST_SOURCES)
    add_executable(unit-tests ${TEST_SOURCES})
    target_include_directories(unit-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    target_link_libraries(unit-tests
        GTest::GTest
        GTest::Main
        ${SDL2_LIBRARIES}
    )
    add_test(NAME UnitTests COMMAND unit-tests)
endif()

# Integration tests
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES
    tests/integration/*.cpp
    tests/integration/*/*.cpp
)

if(INTEGRATION_TEST_SOURCES)
    add_executable(integration-tests ${INTEGRATION_TEST_SOURCES})
    target_include_directories(integration-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    target_link_libraries(integration-tests
        Catch2::Catch2
        Catch2::Catch2WithMain
        ${SDL2_LIBRARIES}
    )
    add_test(NAME IntegrationTests COMMAND integration-tests)
endif()

# Contract tests
file(GLOB_RECURSE CONTRACT_TEST_SOURCES
    tests/contract/*.cpp
)

if(CONTRACT_TEST_SOURCES)
    add_executable(contract-tests ${CONTRACT_TEST_SOURCES})
    target_include_directories(contract-tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    target_link_libraries(contract-tests
        Catch2::Catch2
        Catch2::Catch2WithMain
        ${SDL2_LIBRARIES}
    )
    add_test(NAME ContractTests COMMAND contract-tests)
endif()

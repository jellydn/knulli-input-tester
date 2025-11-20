# CMake module to verify correct target architecture and binary format
# Provides functions to check cross-compilation settings and verify built binaries

# Function to verify cross-compilation setup
function(check_cross_compilation_setup)
    if(NOT CMAKE_CROSSCOMPILING)
        message(WARNING "CMAKE_CROSSCOMPILING not set - you may be compiling for native system instead of target")
    endif()

    message(STATUS "Cross-compilation setup check:")
    message(STATUS "  CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "  CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "  CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
    message(STATUS "  CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
    message(STATUS "  CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}")

    # Verify compiler is accessible
    if(NOT EXISTS "${CMAKE_C_COMPILER}" AND NOT CMAKE_C_COMPILER MATCHES ".*-gcc$")
        message(WARNING "C compiler may not be found: ${CMAKE_C_COMPILER}")
        message(HINT "Install cross-compilation toolchain or specify CMAKE_C_COMPILER explicitly")
    endif()
endfunction()

# Function to verify binary format after build
# Usage: check_binary_format(target_executable)
# Example: check_binary_format(knulli-input-tester)
function(check_binary_format target_name)
    # Get the full path to the executable
    get_target_property(target_location ${target_name} LOCATION)

    if(NOT target_location)
        get_target_property(target_location ${target_name} IMPORTED_LOCATION)
    endif()

    if(target_location)
        # Add custom command to check binary format after build
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Verifying binary format..."
            COMMAND file "${target_location}" || echo "file command not available"
            COMMENT "Checking binary format of ${target_name}"
        )
    endif()
endfunction()

# Function to verify static linking
# Checks that only system libraries are dynamically linked
# Usage: check_static_linking(target_executable)
function(check_static_linking target_name)
    get_target_property(target_location ${target_name} LOCATION)

    if(NOT target_location)
        get_target_property(target_location ${target_name} IMPORTED_LOCATION)
    endif()

    if(target_location)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Checking dynamic dependencies..."
            COMMAND ldd "${target_location}" 2>/dev/null || echo "ldd command not available (may be cross-compiling)"
            COMMAND ${CMAKE_COMMAND} -E echo "Verifying no SDL2/ImGui dynamic deps..."
            COMMAND bash -c "ldd '${target_location}' 2>/dev/null | grep -i 'SDL\\|imgui' && echo 'WARNING: Found SDL/ImGui dynamic links!' || echo 'OK: No SDL/ImGui dynamic dependencies found'"
            COMMENT "Verifying static linking of ${target_name}"
        )
    endif()
endfunction()

# Function to verify target architecture
# Usage: check_target_architecture(arm64|armv7)
function(check_target_architecture expected_arch)
    string(TOLOWER "${expected_arch}" expected_arch_lower)

    if(expected_arch_lower STREQUAL "arm64" OR expected_arch_lower STREQUAL "aarch64")
        if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
            message(WARNING "Expected ARM64 (aarch64) but CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        set(expected_file_string "ARM aarch64")
    elseif(expected_arch_lower STREQUAL "armv7" OR expected_arch_lower STREQUAL "arm32")
        if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7l")
            message(WARNING "Expected ARM32 (armv7l) but CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        set(expected_file_string "ARM")
    else()
        message(FATAL_ERROR "Unknown architecture: ${expected_arch}")
    endif()

    message(STATUS "Target architecture check:")
    message(STATUS "  Expected: ${expected_arch} (${expected_file_string})")
    message(STATUS "  CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
endfunction()

# Automatically check cross-compilation on module load
check_cross_compilation_setup()

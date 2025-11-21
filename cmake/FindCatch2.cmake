# Find Catch2 module
# Sets: CATCH2_FOUND, CATCH2_INCLUDE_DIR

find_package(Catch2 CONFIG QUIET)

if(NOT Catch2_FOUND)
    find_path(CATCH2_INCLUDE_DIR catch2/catch.hpp)
    
    if(CATCH2_INCLUDE_DIR)
        set(CATCH2_FOUND TRUE)
        message(STATUS "Catch2 found: ${CATCH2_INCLUDE_DIR}")
    else()
        set(CATCH2_FOUND FALSE)
    endif()
else()
    message(STATUS "Catch2 found (CONFIG)")
    set(CATCH2_FOUND TRUE)
endif()

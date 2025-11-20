# Find Google Test module
# Sets: GTEST_FOUND, GTEST_LIBRARIES, GTEST_INCLUDE_DIR

find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    find_path(GTEST_INCLUDE_DIR gtest/gtest.h)
    find_library(GTEST_LIBRARY gtest)
    find_library(GTEST_MAIN_LIBRARY gtest_main)
    
    if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY AND GTEST_MAIN_LIBRARY)
        set(GTEST_FOUND TRUE)
        set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
        message(STATUS "Google Test found: ${GTEST_INCLUDE_DIR}")
    else()
        set(GTEST_FOUND FALSE)
    endif()
else()
    message(STATUS "Google Test found (CONFIG)")
    set(GTEST_FOUND TRUE)
endif()

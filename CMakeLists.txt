cmake_minimum_required(VERSION 3.13)
project(ifj2022 CXX C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

# Shamelessly stolen from totok @ https://stackoverflow.com/a/63011172
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

enable_testing()

include_directories(src)
include_directories(test)
add_executable(testScanner
        src/scanner.c src/scanner.h 
        src/error.c src/error.h
        src/vstr.c src/vstr.h
        test/scanner.cpp)
target_link_libraries(testScanner gtest gtest_main)

add_test(scanner testScanner)

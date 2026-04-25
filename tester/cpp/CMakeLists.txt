cmake_minimum_required(VERSION 3.16)
project(cpp_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

set(
    TESTER_CPP_INCLUDE_DIRS
    "${PROJECT_ROOT_DIR}"
    "${PROJECT_ROOT_DIR}/library"
    CACHE STRING "Semicolon-separated include directories for headers under test"
)

include(FetchContent)

# Download and build GoogleTest during configure.
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TESTER_CPP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
list(FILTER TESTER_CPP_SOURCES EXCLUDE REGEX "/build/")

if(NOT TESTER_CPP_SOURCES)
    message(FATAL_ERROR "No C++ test files found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(cpp_test ${TESTER_CPP_SOURCES})

target_include_directories(cpp_test PRIVATE
    ${TESTER_CPP_INCLUDE_DIRS}
)

target_link_libraries(cpp_test PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cpp_test)

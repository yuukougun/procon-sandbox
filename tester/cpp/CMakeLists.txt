cmake_minimum_required(VERSION 3.16)
project(cpp_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

set(
    LIBRARY_DIRS
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

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/unit/*_test.cpp")
list(FILTER TEST_FILES EXCLUDE REGEX "/build/")

if(NOT TEST_FILES)
    message(FATAL_ERROR "No C++ test files found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

include(GoogleTest)

foreach(test_source ${TEST_FILES})
    get_filename_component(test_name ${test_source} NAME_WE)

    # Build one executable per test source. Library .cpp files are not linked here.
    add_executable(${test_name} ${test_source})

    target_include_directories(${test_name} PRIVATE
        ${LIBRARY_DIRS}
    )

    target_link_libraries(${test_name} PRIVATE
        GTest::gtest_main
    )

    gtest_discover_tests(${test_name})
endforeach()

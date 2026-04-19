cmake_minimum_required(VERSION 3.16)
project(array_cpp_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Download and build GoogleTest during configure.
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(array_cpp_test
    array_test.cpp
)

target_include_directories(array_cpp_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../library
)

target_link_libraries(array_cpp_test PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(array_cpp_test)

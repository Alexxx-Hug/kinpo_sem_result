cmake_minimum_required(VERSION 3.16)

project(kinpo_tests_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(kinpo_tests
    tests/tests.cpp
    code/tokenize.cpp
)

target_include_directories(kinpo_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/classes
)

target_link_libraries(kinpo_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(kinpo_tests)
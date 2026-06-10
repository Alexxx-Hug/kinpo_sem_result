cmake_minimum_required(VERSION 3.16)

project(kinpo_tests_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(kinpo_tests
    tests/tests.cpp
)

target_include_directories(kinpo_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/classes
)

target_link_libraries(kinpo_tests PRIVATE
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(kinpo_tests)
cmake_minimum_required(VERSION 3.16)

project(kinpo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kinpo_lib SHARED
    code/application.cpp
    code/file_service.cpp
    code/tokenize.cpp
    code/parser.cpp
    code/expression_solver.cpp
    code/step_formatter.cpp
)

target_include_directories(kinpo_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/classes
)

add_executable(kinpo
    code/main.cpp
)

target_link_libraries(kinpo PRIVATE
    kinpo_lib
)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(kinpo_tests
    tests/tests.cpp
)

target_link_libraries(kinpo_tests PRIVATE
    kinpo_lib
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(kinpo_tests)
cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 20)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# Get all feature files
file(GLOB FEATURES "src/features/*.cpp")
file(GLOB TESTS "tests/*_test.cpp")


# Set up the main function
add_library(features_lib ${FEATURES})
add_executable(RayTracer src/main.cpp)
target_link_libraries(RayTracer features_lib)


# Set up unit testing with Google Test
add_executable(run_tests ${TESTS})
target_link_libraries(run_tests features_lib gtest gtest_main)
enable_testing()
add_test(NAME AllTests COMMAND run_tests)
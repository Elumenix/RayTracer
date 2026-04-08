cmake_minimum_required(VERSION 3.10)
project(RayTracer)

set(CMAKE_CXX_STANDARD 20)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# Fetch stb to allow png images
FetchContent_Declare(
  stb
  URL https://github.com/nothings/stb/archive/master.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(stb)

# Get all feature files
file(GLOB FEATURES "src/features/*.cpp")
file(GLOB TESTS "tests/*_test.cpp")


# Set up the main function
add_library(features_lib ${FEATURES})
target_include_directories(features_lib PUBLIC ${stb_SOURCE_DIR})
add_executable(RayTracer src/main.cpp)
target_link_libraries(RayTracer features_lib)


# Set up unit testing with Google Test
add_executable(run_tests ${TESTS})
target_link_libraries(run_tests features_lib gtest gtest_main)
enable_testing()
add_test(NAME AllTests COMMAND run_tests)
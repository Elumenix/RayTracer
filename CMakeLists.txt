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

# Get all feature files (The .cpp files)
file(GLOB RAYTRACERLIB "src/*.cpp")
file(GLOB TESTS "tests/*_test.cpp")

# Set up the header files
add_library(raytrace_lib ${RAYTRACERLIB}) # Create Library
target_include_directories(raytrace_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include) 
target_include_directories(raytrace_lib PUBLIC ${stb_SOURCE_DIR}) 

# Setup the main executable and give info to the linker
add_executable(RayTracer main.cpp)
target_link_libraries(RayTracer raytrace_lib)


# Set up unit testing with Google Test
add_executable(run_tests ${TESTS})
target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(run_tests raytrace_lib gtest gtest_main)

#target_link_libraries(run_tests raytrace_lib gtest gtest_main)
enable_testing()
add_test(NAME AllTests COMMAND run_tests)
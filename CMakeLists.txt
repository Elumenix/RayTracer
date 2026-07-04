cmake_minimum_required(VERSION 3.10)
project(RayTracer)

# We'll be enforcing C++ 20 standards, and nothing should override that
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing() # Signal to enable testing features
include(FetchContent) # Signal that we will be downloading

# Fetch Google Test if not web build
if (NOT EMSCRIPTEN)
  fetchcontent_declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  fetchcontent_makeavailable(googletest)
endif()


# Fetch stb to allow png images
fetchcontent_declare(
  stb
  URL https://github.com/nothings/stb/archive/master.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
fetchcontent_makeavailable(stb)

# Fetch yaml-cpp so we can parse yaml files
fetchcontent_declare(yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.9.0
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
fetchcontent_makeavailable(yaml-cpp)

# Get all feature files (The .cpp files)
file(GLOB_RECURSE RAYTRACERLIB CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "tests/*_test.cpp")

# We're creating a a library that everything in the RayTracer will use here
add_library(raytrace_lib ${RAYTRACERLIB}) # Create Library
target_include_directories(raytrace_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(raytrace_lib PRIVATE ${stb_SOURCE_DIR})
target_link_libraries(raytrace_lib PUBLIC yaml-cpp::yaml-cpp)

# Setup the main executable and give info to the linker
add_executable(RayTracer main.cpp)
target_link_libraries(RayTracer raytrace_lib)

# if this is a web build, we need to make a .wasm file, a .js file, and move everything in the web folder to the build directory
if (EMSCRIPTEN)
  set_target_properties(RayTracer PROPERTIES 
  OUTPUT_NAME "raytracer"
  SUFFIX ".js"
  )

  target_compile_options(RayTracer PRIVATE -fexceptions)
  target_compile_options(raytrace_lib PRIVATE -fexceptions)
  target_compile_options(yaml-cpp PRIVATE -fexceptions)

  target_link_options(RayTracer PRIVATE 
    -fexceptions
    -sALLOW_MEMORY_GROWTH=1
    -sEXPORT_ES6=1
    -sMODULARIZE=1
    -sENVIRONMENT=worker
    "SHELL:-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','HEAPU8']"
    "SHELL:-sEXPORTED_FUNCTIONS=['_render','_get_width','_get_height','_get_progress','_copy_to_buffer','_get_last_error','_malloc','_free']"
  )
  
  if (EXISTS ${CMAKE_SOURCE_DIR}/web/dist/)
    file(COPY ${CMAKE_SOURCE_DIR}/web/dist/ DESTINATION ${CMAKE_BINARY_DIR})
  endif()
endif()

# Unit testing if this isn't a web build
if (NOT EMSCRIPTEN) 
  # Set up unit testing with Google Test
  # This will be it's own executable, because testing doesn't need stb 
  add_executable(run_tests ${TESTS})
  target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(run_tests raytrace_lib gtest gtest_main)
  add_test(NAME AllTests COMMAND run_tests)
endif()
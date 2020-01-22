cmake_minimum_required(VERSION 3.8)
project(jessilib)

# Set common constants
set(CMAKE_CXX_STANDARD 17)
set(JESSILIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Include compiler-specific build configuration
include(build/CMakeLists.txt)

# Setup source files
add_subdirectory(src)

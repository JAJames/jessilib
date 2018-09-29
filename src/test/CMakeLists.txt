cmake_minimum_required(VERSION 3.8)

# Setup source files
include_directories(../include)
set(SOURCE_FILES
        timer.cpp thread_pool.cpp util.cpp object.cpp)

# Setup gtest
add_subdirectory(googletest/googletest)
#include_directories(googletest/googletest)
#include_directories(googletest/googlemock)

add_executable(jessilib_tests ${SOURCE_FILES})

# Link with gtest
target_link_libraries(jessilib_tests gtest gtest_main jessilib)

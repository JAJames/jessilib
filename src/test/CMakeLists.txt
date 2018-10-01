cmake_minimum_required(VERSION 3.8)

# Setup source files
set(SOURCE_FILES
        timer.cpp thread_pool.cpp util.cpp object.cpp parser.cpp config.cpp parsers/json.cpp unicode.cpp)

# Setup gtest
add_subdirectory(googletest/googletest)
#include_directories(googletest/googletest)
#include_directories(googletest/googlemock)

# Setup executable build target
add_executable(jessilib_tests ${SOURCE_FILES})

# Setup include directories
target_include_directories(jessilib_tests PRIVATE .)

# Link with gtest
target_link_libraries(jessilib_tests gtest gtest_main jessilib)

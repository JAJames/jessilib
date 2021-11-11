# Setup source files
set(SOURCE_FILES
        timer.cpp thread_pool.cpp util.cpp object.cpp parser.cpp config.cpp parsers/json.cpp unicode.cpp app_parameters.cpp io/color.cpp duration.cpp split.cpp split_compilation.cpp)

# Setup gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)
#include_directories(googletest/googletest)
#include_directories(googletest/googlemock)

# Setup executable build target
add_executable(jessilib_tests ${SOURCE_FILES})

# Setup include directories
target_include_directories(jessilib_tests PRIVATE .)

# Link with gtest
target_link_libraries(jessilib_tests gtest gtest_main jessilib)

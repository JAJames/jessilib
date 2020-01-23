# Setup source files
set(SOURCE_FILES
        main.cpp)

# Setup executable build target
add_executable(jessibot ${SOURCE_FILES})

# Setup include directories
target_include_directories(jessibot PRIVATE .)

# Link with jessilib
target_link_libraries(jessibot jessilib)

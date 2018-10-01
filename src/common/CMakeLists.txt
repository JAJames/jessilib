cmake_minimum_required(VERSION 3.8)

# Setup source files
set(SOURCE_FILES
        timer/timer.cpp timer/timer_manager.cpp thread_pool.cpp timer/timer_context.cpp timer/cancel_token.cpp timer/synchronized_timer.cpp object.cpp parser/parser.cpp parser/parser_manager.cpp config.cpp serialize.cpp parsers/json.cpp unicode.cpp)

# Setup library build target
add_library(jessilib ${SOURCE_FILES})

# Setup include directories
target_include_directories(jessilib PUBLIC ../include)
target_include_directories(jessilib PRIVATE ../include/impl/asio/include)

# Setup additionally needed libs
target_link_libraries(jessilib ${JESSILIB_ADDITOINAL_LIBS})

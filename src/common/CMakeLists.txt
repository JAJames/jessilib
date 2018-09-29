cmake_minimum_required(VERSION 3.8)

# Setup source files
include_directories(../include)
set(SOURCE_FILES
        timer/timer.cpp timer/timer_manager.cpp thread_pool.cpp timer/timer_context.cpp timer/cancel_token.cpp timer/synchronized_timer.cpp object.cpp parser/parser.cpp parser/parser_manager.cpp config.cpp serialize.cpp)

add_library(jessilib ${SOURCE_FILES})

target_link_libraries(jessilib ${JESSILIB_ADDITOINAL_LIBS})

# Setup source files
set(SOURCE_FILES
        timer/timer.cpp timer/timer_manager.cpp thread_pool.cpp timer/timer_context.cpp timer/cancel_token.cpp timer/synchronized_timer.cpp object.cpp parser/parser.cpp parser/parser_manager.cpp config.cpp serialize.cpp parsers/json.cpp unicode.cpp io/command.cpp io/command_context.cpp io/message.cpp app_parameters.cpp io/command_manager.cpp)

# Setup library build target
add_library(jessilib ${SOURCE_FILES})

# Setup include directories
target_include_directories(jessilib PUBLIC ../include)
target_include_directories(jessilib PRIVATE ../include/jessilib)
target_include_directories(jessilib PRIVATE ../include/impl/asio/include)

# Setup additionally needed libs
target_link_libraries(jessilib ${JESSILIB_ADDITOINAL_LIBS})

# Setup fmt::fmt; include directories are included as SYSTEM to suppress warnings (which would be treated as errors)
get_target_property(fmt_include_dirs fmt::fmt INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(jessilib SYSTEM PUBLIC ${fmt_include_dirs})
target_link_libraries(jessilib fmt::fmt)

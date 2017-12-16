cmake_minimum_required(VERSION 3.8)

# Setup source files
include_directories(../include)
set(SOURCE_FILES
        timer.cpp timer_manager.cpp)

add_library(jessilib ${SOURCE_FILES})

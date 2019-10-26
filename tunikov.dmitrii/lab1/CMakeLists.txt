cmake_minimum_required(VERSION 3.12.4)
project(inotify)

set(CMAKE_CXX_STANDARD 11)

message("PID_FILE_NAME = " ${PID_FILE_NAME})
if (PID_FILE_NAME)
    add_compile_definitions(PID_FILE_NAME="${PID_FILE_NAME}")
endif()

add_executable(inotify main.cpp core/ConfigHolder.cpp core/ConfigHolder.h core/inotify.hpp core/utils.hpp core/Exception.hpp)

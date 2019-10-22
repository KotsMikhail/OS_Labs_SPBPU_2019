cmake_minimum_required(VERSION 3.4)
project(lab1)

set(CMAKE_CXX_STANDARD 11)

message("PID_FILE = " ${PID_FILE})
if (PID_FILE)
    add_compile_definitions(PID_FILE="${PID_FILE}")
endif()

set(SOURCES
    main.cc
    daemon.cc
    daemon.h
    parser.cc
    parser.h)

add_executable(${PROJECT_NAME} ${SOURCES})

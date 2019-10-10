cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
    main.cc
    daemon.cc
    daemon.h)

add_executable(${PROJECT_NAME} ${SOURCES})

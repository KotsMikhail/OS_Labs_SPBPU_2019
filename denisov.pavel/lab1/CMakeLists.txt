cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 11)
set(SOURCES
    main.cpp
    daemon_utils.cpp
    daemon_utils.h
    bk_files_copy_daemon.cpp
    bk_files_copy_daemon.h)

add_executable(${PROJECT_NAME} ${SOURCES})

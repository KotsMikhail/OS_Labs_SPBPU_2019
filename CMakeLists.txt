cmake_minimum_required(VERSION 3.10)
project(inotify)

set(CMAKE_CXX_STANDARD 14)

add_executable(inotify main.cpp core/ConfigHolder.cpp core/ConfigHolder.h core/inotify.hpp core/utils.hpp)
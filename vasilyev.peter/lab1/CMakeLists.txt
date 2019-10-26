cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(lab1
              "daemon.cpp"
              "daemon.h"
              "main.cpp"
              "config_parser.cpp"
              "config_parser.h")

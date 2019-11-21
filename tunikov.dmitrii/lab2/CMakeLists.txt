cmake_minimum_required(VERSION 3.15)
project(lab2)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab2 main.cpp host/src/host.cpp host/src/host.h interfaces/conn.h host/src/client_data.h interfaces/memory.h)
cmake_minimum_required(VERSION 3.15)
project(lab2)

set(CMAKE_CXX_STANDARD 14)

include_directories(interfaces)

add_executable(lab2 host/host.cpp host/host.h interfaces/conn.h host/client_data.h interfaces/memory.h client/client.cpp client/client.h conn/conn_mq.cpp conn/conn_fifo.cpp)
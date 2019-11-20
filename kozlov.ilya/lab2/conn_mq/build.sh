#!/bin/bash
cmake . -Wall -Werror
make
rm CMakeCache.txt Makefile cmake_install.cmake
rm -r CMakeFiles/
#cp libconn_mq.so ../

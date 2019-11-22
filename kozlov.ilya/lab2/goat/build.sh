#!/bin/bash
function makeWithPostfix {
    cmake . -Wall -Werror -DPOSTFIX=${1}
    make
    rm CMakeCache.txt Makefile cmake_install.cmake
    rm -r CMakeFiles/
    cp client_${1} ../
}
makeWithPostfix "seg"
makeWithPostfix "mq"
makeWithPostfix "fifo"
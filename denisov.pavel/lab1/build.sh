#!/bin/bash

cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=lab1.cbp
if [[ -f "$FILE" ]]; then
	rm "$FILE"
fi
rm -r CMakeFiles/

#!/bin/bash

PID_LOCATION="/var/run/daemon.pid"

if [[ ! -f "$PID_LOCATION" ]]; then
	touch "$PID_LOCATION"
fi

chmod 666 "$PID_LOCATION"

cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=lab1.cbp
if [[ -f "$FILE" ]]; then
	rm "$FILE"
fi
rm -r CMakeFiles/

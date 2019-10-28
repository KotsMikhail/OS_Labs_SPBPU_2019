#!/bin/bash
PID_FILE="/var/run/lab1.pid"
if [ ! -z "$1" ]
  then
    PID_FILE=$1
fi
[[ -f PID_FILE ]] || sudo touch "$PID_FILE"
if [[ $? -ne 0 ]]; then
  echo "touch pid file failed: $?"
  exit $?
fi
sudo chmod 0666 "$PID_FILE"
cmake -Wall -Werror -DPID_FILE="$PID_FILE" .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=lab1.cbp
if [[ -f "$FILE" ]]; then
    rm "$FILE"
fi
rm -r CMakeFiles/

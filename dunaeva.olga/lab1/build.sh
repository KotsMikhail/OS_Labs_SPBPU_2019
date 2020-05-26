#!/bin/bash

absPidPath="/var/run/daemon.pid"

if [[ ! -f $absPidPath ]]
then
  touch $absPidPath
fi

chmod ugo+rw $absPidPath

g++ -std=c++11 -Wall -Werror -o daemon config.h config.cpp daemon.h daemon.cpp main.cpp


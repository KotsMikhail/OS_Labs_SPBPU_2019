#!/bin/bash

absPidPath="/var/run/daemon.pid"

if [[ ! -f $absPidPath ]]
then
  touch $absPidPath
fi

chmod ugo+rw $absPidPath

g++ -Wall -Werror -o daemon main.cpp


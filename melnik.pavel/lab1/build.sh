#!/bin/bash

pid_file="/var/run/mydaemon"

if [[ ! -f "$pid_file" ]]
then
  touch "$pid_file"
fi

chmod 666 "$pid_file"

g++ -Wall -Werror -o melnik_daemon main.cpp

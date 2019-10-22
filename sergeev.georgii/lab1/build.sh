#!/bin/bash

pid_file="/var/run/lab1_pid"

if [[ ! -f "$pid_file" ]]
then
  touch "$pid_file"
fi

chmod 666 "$pid_file"

g++ -Wall -Werror -o lab1_daemon file_dispatcher.h file_dispatcher.cpp main.cpp

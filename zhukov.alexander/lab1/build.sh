#!/bin/bash

pidFile="/var/run/lab1Daemon"

if [[ ! -f "$pidFile" ]]
then
  touch "$pidFile"
fi

chmod 666 "$pidFile"

g++ -std=c++11 -Wall -Werror -o lab1Daemon daemon.cpp
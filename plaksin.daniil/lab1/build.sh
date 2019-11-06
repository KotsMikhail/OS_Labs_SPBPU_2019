#!/bin/bash

pid_location="/var/run/daemon_lab.pid"

if [[ ! -f "$pid_location" ]]
then
  sudo touch "$pid_location"
fi

chmod 666 "$pid_location"

g++ -Wall -Werror -o daemon_lab -lstdc++ -std=c++11 cfg_entry.h cfg_entry.cpp daemon.h daemon.cpp main.cpp
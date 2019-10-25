#!/bin/bash

pid_location="/var/run/daemon_lab.pid"

if [[ ! -f "$pid_location" ]]
then
  sudo touch "$pid_location"
fi

chmod 666 "$pid_location"

g++ -Wall -Werror cfg_entry.h cfg_entry.cpp daemon.cpp -o daemon_lab -lstdc++ -std=c++11
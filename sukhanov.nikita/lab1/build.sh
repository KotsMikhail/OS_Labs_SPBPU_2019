#!/bin/bash

pid_path="/var/run/daemon_lab1.pid"

if [[ ! -f $pid_path ]]
then
  sudo touch $pid_path
fi

sudo chmod ugo+rw $pid_path

g++ -Wall -Werror -o daemon_lab1 Utils.h Utils.cpp Daemon.h Daemon.cpp lab1_main.cpp -std=c++11

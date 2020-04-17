#!/bin/bash

pid_log="/var/run/daemon_lab_5.pid"

if [[ ! -f "$pid_log" ]]
then
 touch "$pid_log"
fi

chmod 666 "$pid_log"

gcc -Wall -Werror main.cpp config_reader.cpp file_deleter.cpp daemon.cpp -o daemon_lab_5 -lstdc++ -std=c++11

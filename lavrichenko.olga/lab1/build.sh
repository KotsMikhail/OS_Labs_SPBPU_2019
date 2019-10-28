#!/bin/bash

pid_log="/var/run/daemon_lab_14.pid"

if [[ ! -f "$pid_log" ]]
then
 touch "$pid_log"
fi

chmod 666 "$pid_log"

gcc -Wall -Werror print_text.cpp -o print_text -lstdc++ -std=c++11
gcc -Wall -Werror daemon_source.cpp -o daemon_lab_14 -lstdc++ -std=c++11
#!/bin/bash

pid_location="/var/run/lab1_daemon.pid"

if [[ ! -f "$pid_location" ]]
then
	touch "$pid_location"
fi

chmod 666 "$pid_location"

gcc -Wall -Werror main.cpp -o Lab1_Daemon -lstdc++
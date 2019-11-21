#!/bin/bash

types_file="types.conf"

for type in $(cat $types_file)
do
g++ -std=c++11 -Wall -Werror message.h clinfo.h connect.h conn_$type.cpp user_host.h user_host.cpp host.cpp -o host_$type -pthread -lrt
done

#!/bin/bash

types_file="types.conf"

for type in $(cat $types_file)
do
g++ -std=c++11 -Wall -Werror message.h connect.h conn_$type.cpp user_client.h user_client.cpp client.cpp -o client_$type -pthread -lrt
done

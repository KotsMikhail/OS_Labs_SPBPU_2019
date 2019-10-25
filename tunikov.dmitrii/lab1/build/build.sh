#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Please, make pid_file_name argument for script"
    exit $?
fi

PID_FILE_NAME=$1
[[ -f pid_file_name ]] || sudo touch "$PID_FILE_NAME"
if [[ $? -ne 0 ]]; then
  echo "can't touch pid_file: $?"
  exit $?
fi
sudo chmod 0666 "$PID_FILE_NAME"

cmake -Wall -Werror -DPID_FILE_NAME="$PID_FILE_NAME" ..
make
CURRENT=$(dirname $0)
rm $CURRENT/CMakeCache.txt $CURRENT/Makefile $CURRENT/cmake_install.cmake
rm -r $CURRENT/CMakeFiles/ 

log_config_tmp="/tmp/log_config"
log_local_location="/var/log/my_log_file.log"
log_params="local0.* $log_local_location"

rm -f "$log_local_location"

rsyslog_location="/etc/rsyslog.conf"
grep -v 'local0.' "$rsyslog_location" > "$log_config_tmp"
mv "$log_config_tmp" "$rsyslog_location"
echo "$log_params" >> "$rsyslog_location"
service rsyslog restart

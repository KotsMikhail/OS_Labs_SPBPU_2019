#!/bin/bash

syslog_location="/etc/syslog.conf"
rsyslog_location="/etc/rsyslog.conf"
tempfile_location="/tmp/lab2_temp"
log_location="/var/log/lab2.log"
log_facility="local1."
log_setting="$log_facility* $log_location"

function change_syslog {
  local location=$( eval "echo \${$1_location}" )
  grep -v $log_facility "$location" > "$tempfile_location"
  mv "$tempfile_location" "$location"
  echo "$log_setting" >> "$location"
  service "$1" restart
}

rm -f "$log_location"

if [[ -f "$rsyslog_location" ]]
then
  change_syslog rsyslog
else
  if [[ ! -f "$syslog_location" ]]
  then
    touch "$syslog_location"
  fi
  change_syslog syslog
fi

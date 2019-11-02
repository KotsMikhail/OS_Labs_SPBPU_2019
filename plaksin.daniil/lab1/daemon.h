#ifndef DAEMON_H
#define DAEMON_H

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <cstdlib>
#include <syslog.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <memory.h>
#include <list>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iterator>
#include <cmath>
#include "cfg_entry.h"

class Daemon
{
public:
    static void hardcore_date_time_validate(std::string date, std::string time);
    static void kill_prev_daemon();
    static void set_pid_file();
    static void signal_handler(int sig);
    static void read_config();
    static void process_config_file(int interval);
    static std::string cfg_path; 

private:
    static const char* PID_FILE;
    static std::list<cfg_entry> cfg_data;
};

#endif //DAEMON_H
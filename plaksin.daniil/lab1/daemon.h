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
    void hardcore_date_time_validate(std::string date, std::string time);
    void kill_prev_daemon();
    void set_pid_file();
    static void signal_handler(int sig);
    void read_config();
    void process_config_file(int interval);
    std::string cfg_path;

    static Daemon* get_instance();

private:
    Daemon();
    Daemon(Daemon const&) = delete;
    Daemon& operator= (Daemon const&) = delete;
    static Daemon* inst;
    const char* PID_FILE;
    std::list<cfg_entry> cfg_data;
};

#endif //DAEMON_H
//
// Created by nero on 24.10.2019.
//

#ifndef DAEMON_H
#define DAEMON_H

#include <string>
#include "file_dispatcher.h"

class Daemon {
private:

    static int interval;

    static FileDispatcher *dispatcher;

public:

    static bool init(const std::string &config_name);

    static void run();

private:

    static void write_pid(std::string pid_file_path);

    static bool file_exist(const std::string &name);

    static int load_config();

    static void terminate(const int &returnCode = EXIT_SUCCESS);

    static void signal_handler(int sig_num);

    static void fork_and_check();

    static void terminate_running_daemon(std::string pid_file_path);

    static void create_daemon();

    static void set_interval(int i) { interval = i; }

    static int get_interval() { return interval; }

};


#endif //DAEMON_H

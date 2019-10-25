//
// Created by nero on 24.10.2019.
//

#include "Daemon.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <csignal>
#include <sys/stat.h>
#include "file_dispatcher.h"

int Daemon::interval = 0;

FileDispatcher * Daemon::dispatcher = FileDispatcher::get_instance();

void Daemon::write_pid(std::string pid_file_path) {
    std::ofstream pid_file(pid_file_path.c_str());
    if (pid_file.is_open()) {
        pid_file << getpid();
        pid_file.close();
    }
}

int Daemon::load_config() {
    int period;
//    FileDispatcher *dispatcher = FileDispatcher::get_instance();
    const std::string &config_file_path = dispatcher->get_config_file_path();
    std::string young_dir_path;
    std::string old_dir_path;
    // create instance of dispatcher with read params
    if (!file_exist(config_file_path)) {
        syslog(LOG_ERR, "Config file %s does not exist.", config_file_path.c_str());
        printf("Config file %s does not exist. \n", config_file_path.c_str());
        return EXIT_FAILURE;
    }
    std::ifstream cfg_reader(config_file_path.c_str());
    if (cfg_reader.is_open() && !cfg_reader.eof()) {
        cfg_reader >> young_dir_path >> old_dir_path >> period;
        cfg_reader.close();
        if (period == 0 || young_dir_path.length() == 0 || old_dir_path.length() == 0) {
            syslog(LOG_ERR, "Config file is incomplete.");
            return EXIT_FAILURE;
        }
        dispatcher->update(young_dir_path, old_dir_path);
        set_interval(period);
    } else {
        syslog(LOG_ERR, "Config file %s does not open.", config_file_path.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}

void Daemon::terminate(const int &returnCode) {
    closelog();
    FileDispatcher::destroy();
    exit(returnCode);
}

void Daemon::signal_handler(int sig_num) {
    switch (sig_num) {
        case SIGHUP:
            syslog(LOG_NOTICE, "Signal -hangup- caught");
            //reload configuration
            if (load_config() != EXIT_SUCCESS) {
                FileDispatcher::destroy();
                exit(EXIT_FAILURE);
            }
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Signal -terminate- caught");
            terminate();
            break;
        default:
            break;
    }
}

void Daemon::fork_and_check() {
    int pid;
    pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "Daemon start failed");
        FileDispatcher::destroy();
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // terminate parent
    }
}

void Daemon::terminate_running_daemon(std::string pid_file_path) {
    std::ifstream pid_file(pid_file_path);
    if (pid_file.is_open() && !pid_file.eof()) {
        pid_t pid;
        pid_file >> pid;
        if (pid > 0) {
            kill(pid, SIGTERM);
        }
        pid_file.close();
    }
}

void Daemon::create_daemon() {
    const std::string pid_file_path = "/var/run/lab1_pid";
    fork_and_check();

    if (setsid() < 0) // not session leader
    {
        syslog(LOG_ERR, "Child process can't become session leader => no session id generated");
        FileDispatcher::destroy();
        exit(EXIT_FAILURE);
    }

    fork_and_check();

    umask(0);

    if (chdir("/") < 0) //change daemon's working dir to root
    {
        syslog(LOG_ERR, "Could not change working directory");
        FileDispatcher::destroy();
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    terminate_running_daemon(pid_file_path);

    write_pid(pid_file_path);
}

bool Daemon::file_exist(const std::string &name) {
    struct stat buffer{};
    return name.length() != 0 && stat(name.c_str(), &buffer) == 0;
}

bool Daemon::init(const std::string &config_name)
{
    std::string config_file_path = realpath(config_name.c_str(), nullptr);
//    FileDispatcher *dispatcher = FileDispatcher::get_instance();
    dispatcher->set_config_file_path(config_file_path);
    // open syslog for daemon
    openlog("lab1", LOG_CONS | LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "Started syslog");

    //read config

    int configuration = load_config();
    if (configuration != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Daemon::run()
{
    create_daemon();
    //FileDispatcher *dispatcher = FileDispatcher::get_instance();
    while (true) {
        dispatcher->process();
        sleep(get_interval());
    }
}
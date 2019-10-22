#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <csignal>
#include <vector>
#include <sys/stat.h>
#include "file_dispatcher.h"

const std::string pid_file_path = "/var/run/lab1_pid";
std::string config_file_path;
FileDispatcher *dispatcher = nullptr;

void clear_memory()
{
    if (dispatcher != nullptr)
        delete (dispatcher);
}

void process()
{
    std::string young_dir = dispatcher->get_young_dir();
    std::string old_dir = dispatcher->get_old_dir();
    dispatcher->process_directories(old_dir, young_dir, OLD_TO_YOUNG);
    dispatcher->process_directories(young_dir, old_dir, YOUNG_TO_OLD);
}

void write_pid()
{
    std::ofstream pid_file(pid_file_path.c_str());
    if (pid_file.is_open())
    {
        pid_file << getpid();
        pid_file.close();
    }
}

bool file_exist(const std::string &name)
{
    struct stat buffer{};
    return name.length() != 0 && stat(name.c_str(), &buffer) == 0;
}

int load_config()
{
    int period;
    std::string young_dir_path;
    std::string old_dir_path;
    // create instance of dispatcher with read params
    if (!file_exist(config_file_path))
    {
        syslog(LOG_ERR, "Config file %s does not exist.", config_file_path.c_str());
        printf("Config file %s does not exist. \n", config_file_path.c_str());
        return EXIT_FAILURE;
    }
    std::ifstream cfg_reader(config_file_path.c_str());
    if (cfg_reader.is_open() && !cfg_reader.eof())
    {
        cfg_reader >> young_dir_path >> old_dir_path >> period;
        cfg_reader.close();
        if (period == 0 || young_dir_path.length() == 0 || old_dir_path.length() == 0)
        {
            syslog(LOG_ERR, "Config file is incomplete.");
            return EXIT_FAILURE;
        }
        if (dispatcher == nullptr)
            dispatcher = new FileDispatcher(young_dir_path, old_dir_path, period);
        else
            dispatcher->update(young_dir_path, old_dir_path, period);
    }
    else
    {
        syslog(LOG_ERR, "Config file %s does not open.", config_file_path.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;

}

void terminate(const int &returnCode = EXIT_SUCCESS)
{
    closelog();
    clear_memory();
    exit(returnCode);
}

void signal_handler(int sig_num) {
    switch (sig_num)
    {
        case SIGHUP:
            syslog(LOG_NOTICE, "Signal -hangup- caught");
            //reload configuration
            if (load_config() != EXIT_SUCCESS)
            {
                clear_memory();
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

void fork_and_check()
{
    int pid;
    pid = fork();
    if (pid == -1)
    {
        syslog(LOG_ERR, "Daemon start failed");
        clear_memory();
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS); // terminate parent
    }
}

void terminate_running_daemon()
{
    std::ifstream pid_file(pid_file_path);
    if (pid_file.is_open() && !pid_file.eof())
    {
        std::string process = "/proc/";
        pid_t pid;
        pid_file >> pid;
        if (pid > 0)
        {
            kill(pid, SIGTERM);
        }
        pid_file.close();
    }
}

void create_daemon()
{
    fork_and_check();

    if (setsid() < 0) // not session leader
    {
        syslog(LOG_ERR, "Child process can't become session leader => no session id generated");
        clear_memory();
        exit(EXIT_FAILURE);
    }

    fork_and_check();

    umask(0);

    if (chdir("/") < 0) //change daemon's working dir to root
    {
        syslog(LOG_ERR, "Could not change working directory");
        clear_memory();
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    terminate_running_daemon();

    write_pid();
}

int main(int argc, char **argv)
{
    int configuration;
    if (argc < 2)
    {
        printf("Error: expected 2 arguments\n");
        return EXIT_FAILURE;
    }
    std::string config_file_name = argv[1];
    config_file_path = realpath(config_file_name.c_str(), nullptr);

    // open syslog for daemon
    openlog("lab1", LOG_CONS | LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "Started syslog");
    //read config
    configuration = load_config();
    if (configuration != EXIT_SUCCESS)
    {
        return configuration;
    }

    create_daemon();

    while (true)
    {
        process();
        sleep(dispatcher->get_interval());
    }
    syslog(LOG_NOTICE, "Close syslog");
    closelog();
    return EXIT_SUCCESS;
}
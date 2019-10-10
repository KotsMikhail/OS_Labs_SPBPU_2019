#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <syslog.h>
#include <sstream>

std::string config_path;
std::string pid_file_path = "/var/run/lab1_daemon";
std::string folder1;
std::string folder2;
unsigned int interval = 0;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

void clear_folder(const std::string& path) {
    system((std::string("rm -rf ") + path).c_str());
    system((std::string("mkdir ") + path).c_str());
}

void copy_bk_files(const std::string& src, const std::string& dst) {
    system((std::string("find ") + src + std::string(" -name \\*.bk  -exec cp {} ") + std::string(dst) + std::string(" \\;")).c_str());
}


bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

void read_config_file() {
    std::ifstream config_file(config_path.c_str());
    if (config_file.is_open() && !config_file.eof()) {
        syslog (LOG_NOTICE, "Reading config file.");
        config_file >> folder1 >> folder2 >> interval;
        config_file.close();
        config_path = realpath(config_path.c_str(), NULL);
    }
}

void signal_handler(int signum) 
{
    if (signum == SIGHUP) {
        syslog (LOG_NOTICE, "SIGHUP signal caught.");
        read_config_file();
    }
    else if (signum == SIGTERM) {
        syslog (LOG_NOTICE, "SIGTERM signal caught.");
        unlink(pid_file_path.c_str());
        exit(0);
    }
}

static void create_daemon()
{
    std::ifstream pid_file(pid_file_path.c_str());

    if (pid_file.is_open() && !pid_file.eof()) {
        syslog (LOG_NOTICE, "Read pid file.");
        pid_t prev_daemon_pid;
        pid_file >> prev_daemon_pid;
        std::string proc = "/proc/" + patch::to_string(prev_daemon_pid);
        if (is_dir(proc.c_str())) {
            syslog (LOG_NOTICE, "Kill previous daemon.");
            kill(prev_daemon_pid, SIGTERM);
        }
    }

    pid_file.close();

    pid_t pid;

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    umask(0);

    if (chdir("/") < 0) {
        syslog (LOG_NOTICE, "Error occured while changing working directory.");
        exit(EXIT_FAILURE);
    }

    std::ofstream pid_file_out(pid_file_path.c_str());

    if (pid_file_out.is_open()) {
        syslog (LOG_NOTICE, "Set pid file.");
        pid_file_out << getpid();
        pid_file_out.close();
    }

}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Error: exprected 2 arguments, got %d", argc);
        return EXIT_FAILURE;
    }

    openlog ("lab1_daemons", LOG_PID, LOG_DAEMON);
    syslog (LOG_NOTICE, "Open syslog");

    config_path = argv[1];

    read_config_file();

    create_daemon();

    while (1)
    {
        syslog (LOG_NOTICE, "Daemon does his work...");
        clear_folder(folder2);
        copy_bk_files(folder1, folder2);
        sleep (interval);
        syslog (LOG_NOTICE, "Daemon finished his work. Daemon is sleeping...");
    }

    syslog (LOG_NOTICE, "Close syslog");
    closelog();

    return EXIT_SUCCESS;
}
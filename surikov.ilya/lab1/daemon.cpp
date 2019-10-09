//
// Created by chopa on 07.10.2019.
//

#include <iostream>
#include <fstream>
#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <cstdlib>
#include <syslog.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>

#define PID_FILE "/var/run/daemon_lab.pid"
#define TEN_MIN 600

enum modes {
    OLDER = 0,
    YOUNGER = 1
};

std::string cfg_path;
std::string folder1;
std::string folder2;
int interval = 0;
bool need_work = true;

void read_config() {
    std::ifstream cfg_file(cfg_path);
    if (!cfg_file.is_open() || cfg_file.eof()) {
        syslog(LOG_ERR, "Could not open config file or it is empty");
        exit(EXIT_FAILURE);
    }
    cfg_file >> folder1 >> folder2 >> interval;
    cfg_file.close();
}

void kill_prev_daemon() {
    std::ifstream ipidFile(PID_FILE);
    if (ipidFile.is_open() && !ipidFile.eof())
    {
        pid_t prev;
        char proc[20] = "/proc/";
        ipidFile >> prev;
        sprintf(proc + 6, "%d", prev);
        struct stat s;
        if (!stat(proc, &s) && S_ISDIR(s.st_mode))
            kill(prev, SIGTERM);
    }
    ipidFile.close();
}

void set_pid_file() {
    std::ofstream pid_file(PID_FILE);
    if (!pid_file) {
        syslog(LOG_ERR, "Could not open pid file");
        exit(EXIT_FAILURE);
    }
    pid_file << getpid();
}

void move_file(const std::string& path_src, const std::string& path_dst) {
    system( (std::string("mv ") + path_src + std::string(" ") + path_dst).c_str() );
}

void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
            need_work = false;
            read_config();
            syslog(LOG_NOTICE, "Hangup Signal Catched");
            need_work = true;
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Terminate Signal Catched");
            unlink(PID_FILE);
            exit(0);
            break;
        default:
            syslog(LOG_NOTICE, "Unknown Signal Catched");
            break;
    }
}


void process_directory(const std::string& folder_src, const std::string& folder_dst, int mode) {
    syslog(LOG_NOTICE, "Successfully working");
    struct stat t_stat;
    time_t creation_time, cur_time;

    DIR* dir;
    struct dirent *ent;
    dir = opendir(folder_src.c_str());
    if (!dir) {
        syslog(LOG_ERR, "Could not open directory: %s", folder_src.c_str());
        exit(EXIT_FAILURE);
    }
    time ( &cur_time );

    while ((ent = readdir(dir)) != nullptr) {
        if (ent->d_type == DT_DIR) {
            continue;
        }
        std::string file_path_src = folder_src + std::string("/") + std::string(ent->d_name);
        std::string file_path_dst = folder_dst + std::string("/") + std::string(ent->d_name);
        stat(file_path_src.c_str(), &t_stat);
        creation_time = t_stat.st_atime;
        switch (mode) {
            case OLDER:
                if ( (cur_time - creation_time) >  TEN_MIN) {
                    move_file(file_path_src, file_path_dst);
                }
                break;
            case YOUNGER:
                if ( (cur_time - creation_time) <  TEN_MIN) {
                    move_file(file_path_src, file_path_dst);
                }
                break;
            default:
                syslog(LOG_NOTICE, "Wrong mode");
        }

    }
    closedir(dir);
}



int main(int argc,char **argv)
{
    pid_t pid = fork(), sid;
    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    if (argc != 2) {
        printf("Wrong number of arguments. Expected: 2. Got: %d", argc);
        return 0;
    }
    cfg_path = argv[1];

    kill_prev_daemon();

    openlog("daemon_lab", LOG_NOWAIT | LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "Successfully started daemon_lab");

    umask(0);

    sid = setsid();
    if (sid < 0)
    {
        syslog(LOG_ERR, "Could not generate session ID for child process");
        exit(EXIT_FAILURE);
    }

    read_config();
    cfg_path = realpath(cfg_path.c_str(), nullptr);

    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Could not change working directory to /");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    set_pid_file();

    while (1)
    {
         if(need_work) {
             process_directory(folder1, folder2, OLDER);
             process_directory(folder2, folder1, YOUNGER);
         }
        sleep(interval);
    }
}
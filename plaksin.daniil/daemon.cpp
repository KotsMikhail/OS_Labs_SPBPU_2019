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
#include <fcntl.h>
#include <string>

#define PID_FILE "/var/run/daemon_lab.pid"
#define ONE_MIN 60
#define ONE_HOUR 3600

std::string cfg_path;
std::string msg_date;
std::string msg_time;
std::string msg_text;
std::string msg_flag;

int interval = 10;
bool need_work = true;

#define ABS(x) ((x) > 0 ? (x) : -(x))

void process_config_file() {
    std::ifstream cfg_file(cfg_path);
    if (!cfg_file.is_open() || cfg_file.eof()) {
        syslog(LOG_ERR, "Could not open config file or it is empty");
        exit(EXIT_FAILURE);
    }

    time_t t_cur_time;
    time(&t_cur_time);

    while (cfg_file >> msg_date >> msg_time >> msg_flag)
    {
        std::cout << "process line" << std::endl;
        if (msg_flag.compare("-h") == 0 || msg_flag.compare("-d") == 0 || msg_flag.compare("-w") == 0 || msg_flag.compare("-m") == 0)
        {
            if (!(cfg_file >> msg_text))
            {
                syslog(LOG_ERR, "Wrong config format");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            msg_text = msg_flag;
            msg_flag = "-m";
        }

	std::cout << "process line with time " << t_cur_time << std::endl;

	if (msg_flag.compare("-m") == 0)
        {
	    int i_msg_time = atoi(msg_time.substr(6, 2).c_str());            

            std::cout << "debug: " << i_msg_time << " " << t_cur_time % 100 << " " << i_msg_time - t_cur_time % 100 << std::endl;
            if ( ABS(i_msg_time - t_cur_time % 100) % ONE_MIN < 10)
            {
                std::cout << msg_text << std::endl;
            }
        }
        else if (msg_flag.compare("-h") == 0)
        {
        }
        else if (msg_flag.compare("-d") == 0)
        {
        }
        else if (msg_flag.compare("-w") == 0)
        {
        }


        
    }
    cfg_file.close();
}

void kill_prev_daemon() {
    std::cout << "kill prev" << std::endl;
    std::ifstream ipidFile(PID_FILE);
    if (ipidFile.is_open() && !ipidFile.eof())
    {
        pid_t prev;
        ipidFile >> prev;
        if (prev > 0) {
            kill(prev, SIGTERM);
        }
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
    pid_file.close();
}

void signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
            need_work = false;
            process_config_file();
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

int main(int argc,char **argv)
{
    pid_t pid = fork();

    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    if (argc < 2) {
        printf("Wrong numbers of arguments. Expected: 2. Got: %d", argc);
        exit(EXIT_FAILURE);
    }

    cfg_path = argv[1];

    openlog("daemon_lab", LOG_NOWAIT | LOG_PID, LOG_USER);

    umask(0);

    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Could not generate session ID for child process");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    cfg_path = realpath(cfg_path.c_str(), nullptr);

    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Could not change working directory to /");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Successfully started daemon_lab");
    std::cout << "pid: " << pid << std::endl;
    close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    kill_prev_daemon();

    set_pid_file();

    while (true)
    {
         if(need_work) {
             std::cout << "work" << std::endl;
             process_config_file();
         }
        sleep(interval);
    }
}
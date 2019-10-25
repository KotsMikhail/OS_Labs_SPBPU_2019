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
#include "cfg_entry.h"

#define PID_FILE "/var/run/daemon_lab.pid"
#define ONE_MIN 60
#define ONE_HOUR 3600

std::string cfg_path;

std::list<cfg_entry> cfg_data;

int interval = 10;
bool need_work = true;

#define ABS(x) ((x) > 0 ? (x) : -(x))

void read_config()
{
    std::string msg_text;
    std::string msg_flag;
    std::string msg_date;
    std::string msg_time;

    std::ifstream cfg_file(cfg_path);

    if (!cfg_file.is_open() || cfg_file.eof()) 
    {
        syslog(LOG_ERR, "Could not open config file or it is empty");
        exit(EXIT_FAILURE);
    }
    //std::cout << timeinfo->tm_sec << std::endl;
    cfg_data.clear();
    while (cfg_file >> msg_date >> msg_time >> msg_flag)
    {
        
        //std::cout << "process line" << std::endl;
        if (msg_flag.compare("-h") == 0 || msg_flag.compare("-d") == 0 || msg_flag.compare("-w") == 0 || msg_flag.compare("-m") == 0)
        {
            if (!(cfg_file >> msg_text))
            {
        std::cout << "fail 1" << std::endl;
                syslog(LOG_ERR, "Wrong config format");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            msg_text = msg_flag;
            msg_flag = "-m";
        }
        struct tm t;
        memset(&t, 0, sizeof t);  
        if (3 != sscanf(msg_date.c_str(),"%d.%d.%d", &t.tm_mday, &t.tm_mon, &t.tm_year)) 
        {
        std::cout << "fail 2" << std::endl;
            syslog(LOG_ERR, "Wrong config format");
            exit(EXIT_FAILURE);
        }
        if (3 != sscanf(msg_time.c_str(),"%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec)) 
        {
        std::cout << "fail 3" << std::endl;
            syslog(LOG_ERR, "Wrong config format");
            exit(EXIT_FAILURE);
        }
            
        t.tm_year -= 1900;
        t.tm_mon--;
            
        if (mktime(&t) < 0) 
        {
        std::cout << "fail 4" << std::endl;
            syslog(LOG_ERR, "Wrong config format");
            exit(EXIT_FAILURE);
        }
        cfg_entry entry(msg_text, msg_flag, t);
        cfg_data.push_back(entry);
        //printf("DOW(%s):%d (0=Sunday, 1=Monday, ...) AND %d %d\n", msg_date.c_str(), t.tm_wday, t.tm_hour, t.tm_min);
    }

    cfg_file.close();
}

void process_config_file() 
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    //std::cout << timeinfo->tm_sec << std::endl;

    for (std::list<cfg_entry>::iterator it = cfg_data.begin(); it != cfg_data.end(); it++)
    {
        if (it->get_entry_flag().compare("-m") == 0)
        {
            //system("gnome-terminal -e 'echo \"sas\"'");//(std::string("gnome-terminal -- 'grep -o \"") + msg_text + std::string("\" ~/.conf'")).c_str());
            //system("gnome-terminal  echo sas");
            
            //system("xterm -e echo sas");
            //std::cout << "sas1" << std::endl;
            if (ABS(timeinfo->tm_sec - it->get_entry_time().tm_sec) <= interval)
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
        else if (it->get_entry_flag().compare("-h") == 0)
        {
            if (timeinfo->tm_min == it->get_entry_time().tm_min && ABS(timeinfo->tm_sec - it->get_entry_time().tm_sec) <= interval) 
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
        else if (it->get_entry_flag().compare("-d") == 0)
        {
            if (timeinfo->tm_hour == it->get_entry_time().tm_hour && timeinfo->tm_min == it->get_entry_time().tm_min && ABS(timeinfo->tm_sec - it->get_entry_time().tm_sec) <= interval) 
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
        else if (it->get_entry_flag().compare("-w") == 0)
        {
            if (timeinfo->tm_wday == it->get_entry_time().tm_wday && timeinfo->tm_hour == it->get_entry_time().tm_hour && timeinfo->tm_min == it->get_entry_time().tm_min && ABS(timeinfo->tm_sec - it->get_entry_time().tm_sec) <= interval)
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }


        
    }
}

void kill_prev_daemon() 
{
    //std::cout << "kill prev" << std::endl;
    std::ifstream ipidFile(PID_FILE);
    if (ipidFile.is_open() && !ipidFile.eof())
    {
        pid_t prev;
        ipidFile >> prev;
        if (prev > 0) 
        {
            kill(prev, SIGTERM);
        }
    }
    ipidFile.close();
}

void set_pid_file() 
{
    std::ofstream pid_file(PID_FILE);
    if (!pid_file) 
    {
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

int main(int argc,char **argv)
{
    pid_t pid = fork();

    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    if (argc < 2)
    {
        printf("Wrong numbers of arguments. Expected: 2. Got: %d\n", argc);
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
    //std::cout << "pid: " << pid << std::endl;
    close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    kill_prev_daemon();

    set_pid_file();
    read_config();
    while (true)
    {
         if(need_work)
         {
             //std::cout << "work" << std::endl;
             process_config_file();
         }
        sleep(interval);
    }
}
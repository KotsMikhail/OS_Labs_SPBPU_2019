#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#include "config.h"
#include "daemon.h"

using namespace std;

Daemon::Daemon(char* conf)
{
    openlog("daemon", LOG_CONS | LOG_PID, LOG_DAEMON);
    Config::GetInstance(conf);
     
    Create();
}

void Daemon::CheckPidFile()
{
    ifstream pidFile(PID_PATH);
    pid_t pidCur;
    pidFile >> pidCur;
    if (!pidFile.eof())
        kill(pidCur, SIGTERM);
    pidFile.close();
}

void Daemon::SetPidToFile()
{
   ofstream pidFile(PID_PATH);
   pidFile << getpid() << endl;
   pidFile.close();
}


void Daemon::Create()
{

    int pid = fork();
    if (pid == -1)
    {
        syslog(LOG_ERR, "Error: Start Daemon failed.\n");
        exit(EXIT_FAILURE);
    }


    if (pid != 0)
       exit(EXIT_SUCCESS);

    umask(0);

    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Error: Sid don't set.\n");
        exit(EXIT_FAILURE);
    }

    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog(LOG_NOTICE, "Daemon created");
    signal(SIGHUP, HandleSignal);
    signal(SIGTERM, HandleSignal);

    CheckPidFile();
    SetPidToFile();
}


void Daemon::HandleSignal(int signal)
{
    switch(signal)
    {
        case SIGHUP:
        {
            syslog(LOG_INFO, "Catched SIGHUP.");
            Config &conf = Config::GetInstance();
            conf.ReadConfig();
            break;
        }
        case SIGTERM:
            syslog(LOG_INFO, "Catched SIGTERM.");
            closelog();
            exit(EXIT_SUCCESS);
        default:
            syslog(LOG_INFO, "Catched unknown signal.");
            break;
    }
}

void Daemon::DeleteDir(string dirPath)
{
    DIR *dir = opendir(dirPath.c_str());

    if (dir == nullptr)
    {
        syslog(LOG_ERR, "Couldn't open subfolder %s.", dirPath.c_str());
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            string subdirPath = dirPath + "/" + entry->d_name;
            string subdirName = entry->d_name;

            if (subdirName != "." && subdirName != "..")
            {
                DeleteDir(subdirPath);
                if (rmdir(subdirPath.c_str()) != 0)
                {
                    syslog(LOG_ERR, "Couldn't delete subfolder.");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
           string filePath = dirPath + "/" + entry->d_name;
           if(unlink(filePath.c_str()) != 0)
            {
                syslog(LOG_ERR, "Couldn't delete subfolder.");
                exit(EXIT_FAILURE);
            }
        }
    }
    closedir(dir);
}


void Daemon::DeleteSubDir()
{
    Config &conf = Config::GetInstance();
    DIR *dir = opendir(conf.GetFolderPath().c_str());
 
    if (dir == nullptr)
    {
        syslog(LOG_ERR, "Couldn't open folder.");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            string Name = entry->d_name;
            if (Name != "." && Name != "..")
            {
                string subdirPath = conf.GetFolderPath() + "/" + entry->d_name;
                DeleteDir(subdirPath);
                rmdir(subdirPath.c_str());
            }
        }
    }

    closedir(dir);
}

void Daemon::Wait()
{
    Config &conf = Config::GetInstance();
    sleep(conf.GetInterval());
}

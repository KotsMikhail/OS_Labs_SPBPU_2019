#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>

#include "config.h"
#include "daemon.h"

using namespace std;

const std::string Daemon::PID_PATH = "/var/run/daemon.pid";

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

void Daemon::Init(char* conf)
{
    openlog("daemon", LOG_CONS | LOG_PID, LOG_DAEMON);
    Config::GetInstance(conf);
}

void Daemon::Create(char* conf)
{
    Init(conf);
    CheckPidFile();


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

void Daemon::DeleteSubDir()
{
    Config &conf = Config::GetInstance();
    DIR *dir = opendir(conf.GetFolderPath().c_str());

    if (dir == nullptr)
    {
        syslog(LOG_ERR, "Error: Couldn't open folder.");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            string subdirPath = conf.GetFolderPath() + "/" + entry->d_name;
            std::system((std::string("rm -r " + subdirPath)).c_str());
        }
    }

    closedir(dir);
}

void Daemon::Wait()
{
    Config &conf = Config::GetInstance();
    sleep(conf.GetInterval());
}

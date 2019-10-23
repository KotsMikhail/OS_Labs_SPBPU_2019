#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>

#include <fstream>

#include "config.h"
#include "daemon.h"

using namespace std;

const string Daemon::ABS_PID_PATH = "/var/run/daemon.pid";

void Daemon::ProtectAgainstRestart()
{
    ifstream pidInputFile(ABS_PID_PATH);
    pid_t pidPrevDaemon;
    pidInputFile >> pidPrevDaemon;
    if (!pidInputFile.eof())
        kill(pidPrevDaemon, SIGTERM);
    pidInputFile.close();

    ofstream pidOutputFile(ABS_PID_PATH);
    pidOutputFile << getpid() << endl;
    pidOutputFile.close();
}

void Daemon::HandleSignal(int sigNum)
{
    switch(sigNum)
    {
    case SIGHUP:
    {
        syslog(LOG_INFO, "Catched SIGHUP.");
        Config &cfg = Config::GetInstance();
        cfg.ReadConfig();
        break;
    }
    case SIGTERM:
        syslog(LOG_INFO, "Catched SIGTERM.");
        syslog(LOG_INFO, "Completed.");
        exit(EXIT_SUCCESS);
    default:
        break;
    }
}

void DeleteFolder(string absFldrPath)
{
    DIR *dir = opendir(absFldrPath.c_str());

    if (dir == nullptr)
    {
        syslog(LOG_ERR, "Couldn't open subfolder %s", absFldrPath.c_str());
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            string subfldrName = entry->d_name;
            if (subfldrName != "." && subfldrName != "..")
                DeleteFolder(absFldrPath + "/" + subfldrName);
        }
        else
        {
            string absFilePath = absFldrPath + "/" + entry->d_name;
            if (remove(absFilePath.c_str()) != 0)
                syslog(LOG_ERR, "Couldn't remove file %s", absFilePath.c_str());
        }
    }

    if (remove(absFldrPath.c_str()) != 0)
        syslog(LOG_ERR, "Couldn't remove subfolder %s", absFldrPath.c_str());

    closedir(dir);
}

void Daemon::DeleteAllSubfolders()
{
    Config &cfg = Config::GetInstance();
    DIR *dir = opendir(cfg.absfldrPath.c_str());

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
            string subfldrName = entry->d_name;
            if (subfldrName != "." && subfldrName != "..")
                DeleteFolder(cfg.absfldrPath + "/" + subfldrName);
        }
    }

    closedir(dir);
}

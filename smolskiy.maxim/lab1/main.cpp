#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <cstdio>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits.h>

using namespace std;

struct Config
{
    string absPath, absfldrPath;
    int interval;

    Config() = default;

    void ReadConfig()
    {
        ifstream cfgFile(absPath);
        cfgFile >> absfldrPath >> interval;

        if (cfgFile.eof())
        {
            syslog(LOG_ERR, "Bad config.");
            exit(EXIT_FAILURE);
        }

        cfgFile.close();
        syslog(LOG_INFO, "Read config.");
    }

    Config(char *cfgName)
    {
        char buf[PATH_MAX];

        if (realpath(cfgName, buf) == nullptr)
        {
            syslog(LOG_ERR, "Config doesn't exist.");
            exit(EXIT_FAILURE);
        }

        absPath = buf;
        ReadConfig();
    }
};

Config cfg;

void ProtectAgainstRestart()
{
    const string ABS_PID_PATH = "/var/run/daemon.pid";

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

void HandleSignal(int sigNum)
{
    switch(sigNum)
    {
    case SIGHUP:
        syslog(LOG_INFO, "Catched SIGHUP.");
        cfg.ReadConfig();
        break;
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

void DeleteAllSubfolders()
{
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Usage: ./daemon config.cfg" << endl;
        exit(EXIT_FAILURE);
    }

    openlog("daemon", LOG_CONS | LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Opened a connection to the system logger.");
    syslog(LOG_INFO, "Run.");

    cfg = Config(argv[1]);

    pid_t pid = fork();

    if (pid == -1)
    {
        syslog(LOG_ERR, "First fork error.");
        exit(EXIT_FAILURE);
    }

    if (pid != 0)
        exit(EXIT_SUCCESS);
    
    if (setsid() == (pid_t)(-1))
    {
        syslog(LOG_ERR, "Setsid error.");
        exit(EXIT_FAILURE);
    }

    umask(0);

    pid = fork();

    if (pid == -1)
    {
        syslog(LOG_ERR, "Second fork error.");
        exit(EXIT_FAILURE);
    }

    if (pid != 0)
        exit(EXIT_SUCCESS);
    
    if (chdir("/") == -1)
    {
        syslog(LOG_ERR, "Chdir error.");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, HandleSignal);
    signal(SIGTERM, HandleSignal);

    ProtectAgainstRestart();

    while (true)
    {
        DeleteAllSubfolders();
        sleep(cfg.interval);
    }
}

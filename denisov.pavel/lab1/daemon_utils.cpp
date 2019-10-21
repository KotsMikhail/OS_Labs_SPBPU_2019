#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <pwd.h>

#include "common.h"
#include "daemon_utils.h"

void Fork ()
{
    pid_t pid = fork();
    if (pid < 0) {
        LOG_ERROR_AND_EXIT("Error occured in the fork while daemonising. Error number is %d", errno);
    }

    if (pid > 0) {
        INFO("Fork successful. (Parent process) PID: %d", pid);
        exit(0);
    }
    INFO("Fork successful. (Child process) PID: %d", pid);
}

pid_t CreateSession ()
{
    pid_t sid = setsid();
    if (sid < 0) {
        LOG_ERROR_AND_EXIT("Error occured in making a new session while daemonising. Error number is %d", errno);
    }
    INFO("New session was successfully created!");
    return sid;
}

void RedirectStdIO ()
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    if (open("/dev/null", O_RDONLY) == -1) {
        LOG_ERROR_AND_EXIT("Failed to reopen stdin while daemonising. Error number is %d", errno);
    }

    if (open("/dev/null", O_WRONLY) == -1) {
        LOG_ERROR_AND_EXIT("Failed to reopen stdout while daemonising. Error number is %d", errno);
    }

    if (open("/dev/null", O_RDWR) == -1) {
        LOG_ERROR_AND_EXIT("Failed to reopen stderr while daemonising. Error number is %d", errno);
    }
}

bool IsProcessRunning (pid_t pid)
{
    struct stat sts;
    std::string pathToDaemon = "/proc/" + std::to_string(pid);
    return !(stat(pathToDaemon.c_str(), &sts) == -1 && errno == ENOENT);
}

bool IsDirectoryExist (std::string &dirPath)
{
    if (dirPath[0] == '~') {
        passwd *pw;
        uid_t uid;

        uid = geteuid();
        pw = getpwuid(uid);
        if (pw != nullptr) {
            dirPath.replace(0, 1, std::string("/home/") + pw->pw_name);
        } else {
            WARNING("Couldn't find username by UID %d. There is no guarantee to find folder which path contains '~'.", uid);
        }
    }

    struct stat sts;
    return (dirPath.length() != 0 && stat(dirPath.c_str(), &sts) == 0 && S_ISDIR(sts.st_mode));
}

void CreatePidFileAndWritePid (const std::string &pidFilePath)
{
    INFO("Trying to create file \'%s\'", pidFilePath.c_str());
    std::ofstream pidFile;
    pidFile.open(pidFilePath, std::ofstream::out | std::ofstream::trunc);
    if (!pidFile.is_open()) {
        LOG_ERROR_AND_EXIT("Failed to create a pid file \'%s\' while daemonising. Error number is %d", pidFilePath.c_str(), errno);
    }

    pidFile << getpid();
    pidFile.close();
}

void RemovePidFile (const std::string &pidFilePath)
{
    INFO("Trying to remove file \'%s\'", pidFilePath.c_str());
    if (remove(pidFilePath.c_str()) != 0) {
        LOG_ERROR_AND_EXIT("Failed to remove the pid file. Error number is %d", errno);
    }
}

void StopRunningByPID (pid_t pid)
{
    INFO("Running process with pid %d killing process started...", pid);

    if (pid <= 0) {
        WARNING("Corrupted process ID found: %d", pid);
        return;
    }

    if (IsProcessRunning(pid)) {
        INFO("Try to kill process with id: %d", pid);
        kill(pid, SIGTERM);
    }
    INFO("Running process with pid %d was killed", pid);
}

void CheckPidFile (const std::string &pidFilePath)
{
    INFO("Checking pid file...");
    std::ifstream pidFile(pidFilePath);
    if (pidFile) {
        // If it founded, try to read a pid-number
        pid_t pidInFile;
        pidFile >> pidInFile;
        if (IsProcessRunning(pidInFile)) {
            StopRunningByPID(pidInFile);
        }
        pidFile.close();

        // Recreating the pid file to clear all information in it
        RemovePidFile(pidFilePath);
        CreatePidFileAndWritePid(pidFilePath);
    } else {
        // If the file doesn't exist, create it and write the currents process's pid to the file
        INFO("The pid-file not found");
        CreatePidFileAndWritePid(pidFilePath);
    }
}

void SetRootAsWorkingDirectory ()
{
    if ((chdir("/")) < 0) {
        LOG_ERROR_AND_EXIT("Failed to change working directory while daemonising. Error number is %d", errno);
    }
}

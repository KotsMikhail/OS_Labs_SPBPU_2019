#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <pwd.h>
#include <syslog.h>

#include "daemon_utils.h"

void Fork ()
{
    pid_t pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Error occured in the fork while daemonising. Error number is %d", errno);
        exit(SIGTERM);
    }

    if (pid > 0) {
        exit(0);
    }
}

pid_t CreateSession ()
{
    pid_t sid = setsid();
    if (sid < 0) {
        syslog(LOG_ERR, "Error occured in making a new session while daemonising. Error number is %d", errno);
        exit(SIGTERM);
    }
    return sid;
}

void RedirectStdIO ()
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    if (open("/dev/null", O_RDONLY) == -1) {
        syslog(LOG_ERR, "Failed to reopen stdin while daemonising. Error number is %d", errno);
        exit(SIGTERM);
    }

    if (open("/dev/null", O_WRONLY) == -1) {
        syslog(LOG_ERR, "Failed to reopen stdout while daemonising. Error number is %d", errno);
        exit(SIGTERM);
    }

    if (open("/dev/null", O_RDWR) == -1) {
        syslog(LOG_ERR, "Failed to reopen stderr while daemonising. Error number is %d", errno);
        exit(SIGTERM);
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
            syslog(LOG_WARNING, "Couldn't find username by UID %d. There is no guarantee to find folder which path contains '~'.", uid);
        }
    }

    struct stat sts;
    return (dirPath.length() != 0 && stat(dirPath.c_str(), &sts) == 0 && S_ISDIR(sts.st_mode));
}

void WritePidToFile (const std::string &pidFilePath)
{
    std::ofstream pidFile;
    pidFile.open(pidFilePath, std::ofstream::out | std::ofstream::trunc);
    if (!pidFile.is_open()) {
        syslog(LOG_ERR, "Failed to open a pid file \'%s\' while daemonising. Error number is %d", pidFilePath.c_str(), errno);
        exit(SIGTERM);
    }

    pidFile << getpid();
    pidFile.close();
}

void StopRunningByPID (pid_t pid)
{
    if (pid <= 0) {
        syslog(LOG_WARNING, "Corrupted process ID found: %d", pid);
        return;
    }

    if (IsProcessRunning(pid)) {
        kill(pid, SIGTERM);
    }
}

bool CheckPidFile (const std::string &pidFilePath)
{
    std::ifstream pidFile(pidFilePath);
    if (!pidFile) {
        return false;
    }

    // If it founded, try to read a pid-number
    pid_t pidInFile;
    pidFile >> pidInFile;
    StopRunningByPID(pidInFile);
    pidFile.close();

    WritePidToFile(pidFilePath);

    return true;
}

void SetRootAsWorkingDirectory ()
{
    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Failed to change working directory while daemonising. Error number is %d", errno);
        exit(SIGTERM);
    }
}

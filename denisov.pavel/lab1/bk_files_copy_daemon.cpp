#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <cerrno>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <iostream>
#include <ftw.h>
#include <functional>
#include <dirent.h>
#include <syslog.h>

#include "daemon_utils.h"
#include "bk_files_copy_daemon.h"

static const int s_supportedSignals[] = { SIGHUP, SIGTERM };

static const std::string s_pidFilePath = "/var/run/daemon.pid";

static std::string s_configFilePath;

static std::string s_srcDir;
static std::string s_dstDir;
static int s_sleepInterval = 0;

static const int s_fdLimit = 64;

static const std::string s_neededFileExt = "bk";

using FILE_DETECTOR_FUNC = std::function<bool(const std::string&)>;


static bool _ReadConfigFile (const std::string &configFilePath)
{
    std::fstream instream(configFilePath.c_str());
    if (!instream.is_open()) {
        return false;
    }

    getline(instream, s_srcDir);
    if (s_srcDir.length() == 0) {
        syslog(LOG_ERR, "Error! Failed to parse the config file. First line is empty, bui it should be three not-empty lines.");
        exit(SIGTERM);
    }

    getline(instream, s_dstDir);
    if (s_dstDir.length() == 0) {
        syslog(LOG_ERR, "Error! Failed to parse the config file. Second line is empty, bui it should be three not-empty lines.");
        exit(SIGTERM);
    }

    // check directories existing
    if (!IsDirectoryExist(s_srcDir)) {
        syslog(LOG_ERR, "Error! Directory %s doesn't exist.", s_srcDir.c_str());
        exit(SIGTERM);
    }
    if (!IsDirectoryExist(s_dstDir)) {
        syslog(LOG_ERR, "Error! Directory %s doesn't exist.", s_dstDir.c_str());
        exit(SIGTERM);
    }

    std::string line;
    getline(instream, line);
    if (line.length() == 0) {
        syslog(LOG_ERR, "Error! Failed to parse the config file. Third line is empty, bui it should be three not-empty lines.");
        exit(SIGTERM);
    }
    s_sleepInterval = atoi(line.c_str());

    return true;
}

static void _HandleSIGHUP ()
{
    // Reread configuration file
    if (!_ReadConfigFile(s_configFilePath)) {
        syslog(LOG_ERR, "Error! Failed to read config file %s", s_configFilePath.c_str());
        exit(SIGTERM);
    }
}

static void _SignalHandler (int signalNumber)
{
    switch (signalNumber) {
        case SIGTERM:
            syslog(LOG_INFO, "SIGTERM has been caught! Exiting...");
            exit(SIGTERM);
            break;
        case SIGHUP:
            _HandleSIGHUP();
            break;
        default:
            syslog(LOG_WARNING, "Get unsupported signal %d", signalNumber);
    }
}

static void _HandleSignals ()
{
    for (int supportedSignal : s_supportedSignals) {
        if (signal(supportedSignal, _SignalHandler) == SIG_ERR) {
            syslog(LOG_ERR, "Error! Can't handle signal number %d", supportedSignal);
            exit(SIGTERM);
        }
    }
}

static void _StopRunningIfOpened ()
{
    SetRootAsWorkingDirectory();
    // Try to find pid-file
    std::ifstream pidFile(s_pidFilePath);
    if (!pidFile) {
        syslog(LOG_WARNING, "The pid-file not found.");
        return;
    }

    // If it founded, try to read a pid-number and kill process with that ID
    pid_t pidInFile;
    pidFile >> pidInFile;
    StopRunningByPID(pidInFile);

    pidFile.close();
}

static int _rmFiles (const char *pathName, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (ftwb->level == 0) {
        return 0;
    }

    int removeRes = remove(pathName);
    if (removeRes != 0) {
        syslog(LOG_ERR, "Error! Failed to remove by path: %s. Error number is %d", pathName, errno);
        exit(SIGTERM);
    }

    return removeRes;
}

static void _ClearDirWithAllSubDirs (std::string& dirPath)
{
    if (!IsDirectoryExist(dirPath)) {
        syslog(LOG_ERR, "Error! Directory %s doesn't exist.", dirPath.c_str());
        exit(SIGTERM);
    }
    nftw(dirPath.c_str(), _rmFiles, s_fdLimit, FTW_DEPTH | FTW_PHYS);
}

static bool _CopyFile (const std::string &srcPath, const std::string &dstPath)
{
    std::ifstream srcFile(srcPath.c_str(), std::ios::binary);
    if (!srcFile.good()) {
        syslog(LOG_ERR, "Error! Source file \'%s\' doesn't exist", srcPath.c_str());
        return false;
    }

    std::ofstream dstFile(dstPath.c_str(), std::ios::binary);
    dstFile << srcFile.rdbuf();

    srcFile.close();
    dstFile.close();

    return true;
}

static void _CopyFilesFromSrcToDstDir (std::string &srcDir, std::string &dstDir, const FILE_DETECTOR_FUNC &detFunc)
{
    if (!IsDirectoryExist(dstDir)) {
        syslog(LOG_ERR, "Error! Directory %s doesn't exist.", dstDir.c_str());
        exit(SIGTERM);
    }

    if (!IsDirectoryExist(srcDir)) {
        syslog(LOG_ERR, "Error! Directory %s doesn't exist.", srcDir.c_str());
        exit(SIGTERM);
    }

    DIR *srcDirHandler;
    struct dirent *ent;

    if ((srcDirHandler = opendir(srcDir.c_str())) != nullptr) {
        while ((ent = readdir(srcDirHandler)) != nullptr) {
            if (ent->d_type == DT_DIR) {
                continue;
            }

            std::string fileName(ent->d_name);
            if (detFunc(fileName)) {
                std::string srcPath = srcDir + "/" + ent->d_name;
                std::string dstPath = dstDir + "/" + ent->d_name;
                _CopyFile(srcPath, dstPath);
            }
        }

        closedir(srcDirHandler);
    } else {
        syslog(LOG_ERR, "Error! Failed to open source directory: %s. Error number: %d", srcDir.c_str(), errno);
        exit(SIGTERM);
    }
}

static bool _CheckFileExt (const std::string& fileName)
{
    return fileName.substr(fileName.find_last_of(".") + 1) == s_neededFileExt;
}

static void _DoJob ()
{
    _ClearDirWithAllSubDirs(s_dstDir);
    _CopyFilesFromSrcToDstDir(s_srcDir, s_dstDir, _CheckFileExt);
}

void Daemonise ()
{
    syslog(LOG_INFO, "Starting daemonisation...");

    Fork();
    CreateSession();
    Fork();
    syslog(LOG_INFO, "Successfully forked and created new session...");

    SetRootAsWorkingDirectory();
    // Grant all permissions for all files and directories created by the daemon
    umask(0);

    _StopRunningIfOpened();

    RedirectStdIO();
    if (!CheckPidFile(s_pidFilePath)) {
        syslog(LOG_ERR, "Error! Pid files %s doesn't exist.", s_pidFilePath.c_str());
        exit(SIGTERM);
    }

    _HandleSignals();

    syslog(LOG_INFO, "Daemonisation finished successfully.");
}

void DaemonWorkLoop ()
{
    syslog(LOG_INFO, "Starting working...");
    while (true) {
        _DoJob();
        sleep(s_sleepInterval);
    }
}

bool LoadConfig (const std::string &configFilePath)
{
    s_configFilePath = configFilePath;
    return _ReadConfigFile(s_configFilePath);
}

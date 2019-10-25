#include <iostream>
#include <fstream>
#include <syslog.h>
#include <unistd.h>
#include <ftw.h>
#include <signal.h>
#include <dirent.h>

#define THREE_MIN 180

std::string pidFilePath = "/var/run/lab1Daemon";
std::string configFilePath;
std::string folder1;
std::string folder2;

unsigned int timeInterval = 0;

void ReadConfig() {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        syslog(LOG_ERR, "Could not open config file!");
        exit(EXIT_FAILURE);
    }
    if (configFile.eof()) {
        syslog(LOG_ERR, "Config file is empty!");
        exit(EXIT_FAILURE);
    }
    configFile >> folder1 >> folder2 >> timeInterval;
    if (folder1.length() == 0 || folder2.length() == 0 || timeInterval == 0) {
        syslog(LOG_ERR, "Incorrect config data!");
        exit(EXIT_FAILURE);
    }
    configFile.close();
}

void KillPreviousDaemon() {
    std::ifstream pidFile(pidFilePath);
    if (pidFile.is_open() && !pidFile.eof()) {
        pid_t pid;
        pidFile >> pid;
        if (pid > 0) {
            kill(pid, SIGTERM);
        }
    }
    pidFile.close();


}

void SignalHandler(int signal) {
    if (signal == SIGHUP) {
        syslog(LOG_NOTICE, "SIGHUP signal caught");
        ReadConfig();
    } else if (signal == SIGTERM) {
        syslog(LOG_NOTICE, "SIGTERM signal caught");
        unlink(pidFilePath.c_str());
        exit(0);
    } else {
        syslog(LOG_NOTICE, "Unknown signal caught");
    }
}

void CreateDaemon() {
    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    if (setsid() < 0) {
        syslog(LOG_ERR, "Could not generate id for child process");
        exit(EXIT_FAILURE);
    }
    umask(0);
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "Could not change working directory");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog(LOG_NOTICE, "Daemon started");
    signal(SIGHUP, SignalHandler);
    signal(SIGTERM, SignalHandler);

    KillPreviousDaemon();

    std::ofstream pidFile(pidFilePath);
    if (!pidFile.is_open()) {
        syslog(LOG_ERR, "Could not open pid file!");
        exit(EXIT_FAILURE);
    } else {
        pidFile << getpid();
        pidFile.close();
    }
}


void CopyFile(const std::string &source, const std::string &destination) {
    std::ifstream srcFile(source);
    std::ofstream dstFile(destination);
    dstFile << srcFile.rdbuf();
}

bool FolderExist(const std::string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

void ClearFolder(const std::string &path) {
    DIR *theFolder;
    if (!(theFolder = opendir(folder1.c_str()))) {
        syslog(LOG_ERR, "Could not open directory: %s", folder1.c_str());
        exit(EXIT_FAILURE);
    }
    struct dirent *nextFile;
    while ((nextFile = readdir(theFolder)) != nullptr) {
        std::string filePath = path + std::string("/") + std::string(nextFile->d_name);
        remove(filePath.c_str());
    }
    closedir(theFolder);
}

void DaemonProcess() {
    DIR *theFolder;
    time_t creationTime, currentTime;
    time(&currentTime);
    struct stat fileStat;
    struct dirent *nextFile;

    if (!FolderExist(folder1)) {
        syslog(LOG_ERR, "Source directory %s don't exist", folder1.c_str());
        exit(EXIT_FAILURE);
    }

    if (!FolderExist(folder2)) {
        syslog(LOG_ERR, "Destination directory %s don't exist", folder2.c_str());
        exit(EXIT_FAILURE);
    }

    if (!(theFolder = opendir(folder1.c_str()))) {
        syslog(LOG_ERR, "Could not open directory: %s", folder1.c_str());
        exit(EXIT_FAILURE);
    }
    std::string newFilesDir = folder2 + std::string("/NEW");
    std::string oldFilesDir = folder2 + std::string("/OLD");
    mkdir(newFilesDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir(oldFilesDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    ClearFolder(newFilesDir);
    ClearFolder(oldFilesDir);
    while ((nextFile = readdir(theFolder)) != nullptr) {
        if (nextFile->d_type != DT_DIR) {
            std::string sourcePath = folder1 + std::string("/") + std::string(nextFile->d_name);
            std::string destPath = folder2 + std::string("/") + std::string(nextFile->d_name);
            stat(sourcePath.c_str(), &fileStat);
            creationTime = fileStat.st_atime;
            if ((currentTime - creationTime) < THREE_MIN) {
                std::string destPath = folder2 + std::string("/NEW/") + std::string(nextFile->d_name);
                CopyFile(sourcePath, destPath);
            } else {
                std::string destPath = folder2 + std::string("/OLD/") + std::string(nextFile->d_name);
                CopyFile(sourcePath, destPath);
            }
        }
    }
    closedir(theFolder);

}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Wrong numbers of arguments. Expected 2 arguments");
        exit(EXIT_FAILURE);
    }
    configFilePath = realpath(argv[1], nullptr);
    openlog("Lab1_daemon", LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "Log opened");
    ReadConfig();
    CreateDaemon();
    while (true) {
        DaemonProcess();
        sleep(timeInterval);
    }
}
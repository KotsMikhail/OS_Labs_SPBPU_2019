#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <string> 
#include <sys/file.h>
#include <execinfo.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <syslog.h>

#include <fstream>
#include <sys/types.h>
#include <syslog.h>
#include <pwd.h>
#include <ftw.h>
#include <string.h>
#include <sstream>
#include <glob.h>
using namespace std;

static const int MAXLEVEL = 1;
static const string pidFilePath = "/var/run/mydaemon";

string dirPath;
int interval = -1;
string realConfigPath;

bool checkFileExist (const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

void readConfigFile() {
    if(!checkFileExist(realConfigPath)) {
        cout << "Couldn't find config file";
        syslog (LOG_NOTICE, "Couldn't find config file");
        exit(EXIT_FAILURE);
    } 

    ifstream configFile(realConfigPath);
    if(configFile.is_open() && !configFile.eof()) {
        configFile >> dirPath;
        configFile >> interval;
    }
    if(dirPath.length() == 0 || interval == -1) {
        cout << "Couldn't read parametrs from config file";
        syslog (LOG_NOTICE, "Couldn't read parametrs from config file");
        exit(EXIT_FAILURE);
    }
    if(interval < 0) {
        cout << "Interval cannot be negative";
        syslog (LOG_NOTICE, "Negative inteval entered");
        exit(EXIT_FAILURE);
    }
    configFile.close();

    dirPath = realpath(dirPath.c_str(), NULL);
}

pid_t fullFork() {
    pid_t pid, sid;

    /* отделяемся от родительского процесса */       
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* Если с PID'ом все получилось, то родительский процесс можно завершить. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Создание нового SID для дочернего процесса */
    sid = setsid();
    if (sid < 0) {
    /* Журналируем любой сбой */
        exit(EXIT_FAILURE);
    }

    /* отделяемся от родительского процесса */       
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* Если с PID'ом все получилось, то родительский процесс можно завершить. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Изменяем файловую маску */
    umask(0);

    /* Изменяем текущий рабочий каталог */
    if ((chdir("/")) < 0) {
    /* Журналируем любой сбой */
        syslog (LOG_NOTICE, "Cannot change directory");
        exit(EXIT_FAILURE);
    }

    /* Закрываем стандартные файловые дескрипторы */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return pid;
}

void slot(int signal) {
    if (signal == SIGHUP) {
        readConfigFile();
    } else if (signal == SIGTERM) {
        syslog (LOG_NOTICE, "End executing because of SIGTERM");
        closelog();
        exit(EXIT_SUCCESS);
    }
}

void setSignalSlots() {
    signal(SIGHUP, slot);
    signal(SIGTERM, slot);
}

void killSameDaemon() {
    pid_t prevDaemonPid;
    if(!checkFileExist(pidFilePath)) {
        return;
    }

    ifstream pidFile(pidFilePath);
    if(pidFile.is_open() && !pidFile.eof()) {
        pidFile >> prevDaemonPid;
    }
    if(prevDaemonPid > 0) {
        syslog (LOG_NOTICE, "Kill existing daemon");
        kill(prevDaemonPid, SIGTERM);
    }
    pidFile.close();
}

void fillPidFile() {
    ofstream pidFile(pidFilePath);
   
    if (pidFile.is_open()) {
        pidFile << getpid();
    }
    else {
        syslog (LOG_NOTICE, "Couldn't open pid file");
        exit(EXIT_FAILURE);
    }
    pidFile.close();
}

bool isDirectory(string path) {
    struct stat buffer;
    if (path.length() != 0 && stat(path.c_str(), &buffer) == 0 && (S_ISDIR(buffer.st_mode))) {
        return true;
    }
    return false;
}

int removeSubdir(const char* pathname, const struct stat *sbuf, int type, struct FTW *ftwb) {

    if (pathname == dirPath) {
        return 0;
    }

    if (ftwb->level > MAXLEVEL) {
        if (remove(pathname) < 0) {
            syslog (LOG_NOTICE, "Error removing file");
            return -1;
        } 
        
   }
   else if (isDirectory(pathname)) {
        if (remove(pathname) < 0) {
            syslog (LOG_NOTICE, "Error removing file");
            return -1;
        }
   }
   return 0;
}

void removeSubdirectories() {
    if (isDirectory(dirPath)) {
        nftw(dirPath.c_str(), removeSubdir, 10, FTW_DEPTH|FTW_MOUNT|FTW_PHYS);
    }
}

int main(int argc, char **argv)
{   
    if(argc != 2) {
        cout << "Wrong arguments count" << endl;
        return EXIT_FAILURE;
    }

    openlog ("mydaemon", LOG_PID, LOG_DAEMON);
    syslog (LOG_NOTICE, "Open syslog");

    string configPath = argv[1];

    realConfigPath = realpath(configPath.c_str(), NULL);

    readConfigFile();

    fullFork();

    setSignalSlots();

    killSameDaemon();

    fillPidFile();
   
    while (true)
    {
        removeSubdirectories();
        sleep(interval);
    }

    syslog (LOG_NOTICE, "Close syslog");
    closelog();

    return EXIT_SUCCESS;
}

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include "Reminder.h"

static const char*  PID_LOGS = "/var/run/daemon_lab_14.pid";
static const int SLEEP_TIME = 1;

void update_pid_log() {
    std::ifstream iFile(PID_LOGS);
    if (iFile.is_open() && !iFile.eof()) {
        pid_t p;
        iFile >> p;
        if (p > 0)
            kill(p, SIGTERM);
    }
    iFile.close();
    std::ofstream oFile(PID_LOGS);
    if (!oFile) {
        syslog(LOG_ERR, "Could not open pid logs");
        //exit(EXIT_FAILURE);
    } else {
        oFile << getpid();
        oFile.close();
    }
}


void signal_handler(int sgnl) {
    Reminder &reminder = Reminder::getInstance();
    switch (sgnl) {
        case SIGHUP:
            reminder.read();
            syslog(LOG_NOTICE, "SIGHUP catched");
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "SIGTERM catched");
            unlink(PID_LOGS);
            exit(0);
            break;
    }
}


int main(int argc, char** argv) {
    pid_t pid = fork();
    switch (pid) {
        case 0:
            break;
        case -1:
            exit(EXIT_FAILURE);
        default:
            syslog(LOG_NOTICE, "Soccessfully made fork. Child's pid is %d.", pid);
            exit(EXIT_SUCCESS);
    }

    if (argc < 2) {
        printf("Expected arguments.");
        exit(EXIT_FAILURE);
    }

    openlog("daemon_lab_14", LOG_NOWAIT | LOG_PID, LOG_USER);
    umask(0);

    if (setsid() < 0) {
        syslog(LOG_ERR, "Could not generate session process");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    switch (pid) {
        case 0:
            break;
        case -1:
            exit(EXIT_FAILURE);
        default:
            syslog(LOG_NOTICE, "Soccessfully made fork. Child's pid is %d.", pid);
            exit(EXIT_SUCCESS);
    }

    Reminder &reminder = Reminder::getInstance();
    std::string config_path = realpath(argv[1], nullptr);
    reminder.setPath(config_path);

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Could not change directory to /");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Success config_path:%s", config_path.c_str());

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    update_pid_log();

    while (true) {
        reminder.exec();
        sleep(SLEEP_TIME);
    }

    return 0;
}

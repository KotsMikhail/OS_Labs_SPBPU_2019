#include "daemon.h"


const char* PID_LOGS = "/var/run/daemon_lab_5.pid";


void Daemon::make_fork() {
    pid_t pid = fork();
    switch (pid) {
        case 0:
            break;
        case -1:
            exit(EXIT_FAILURE);
        default:
            syslog(LOG_NOTICE, "Forked. Child's pid is %d.", pid);
            exit(EXIT_SUCCESS);
    }
}

void signal_handler(int sgnl) {
    switch (sgnl) {
        case SIGHUP:
            Daemon::getInstance().update();
            syslog(LOG_NOTICE, "SIGHUP catched");
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "SIGTERM catched");
            unlink(PID_LOGS);
            exit(EXIT_SUCCESS);
            break;
    }
}

void Daemon::init(int argc, char** argv) {
    make_fork();

    if (argc < 2) {
        printf("Expected arguments.");
        exit(EXIT_FAILURE);
    }

    openlog("daemon_lab_5", LOG_NOWAIT | LOG_PID, LOG_USER);
    umask(0);

    if (setsid() < 0) {
        syslog(LOG_ERR, "Could not generate session process");
        exit(EXIT_FAILURE);
    }

    make_fork();

    config.setPath(realpath(argv[1], nullptr));
    update();

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Could not change directory to /");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    update_pid_log();
}

void Daemon::update_pid_log() {
    std::ifstream iFile(PID_LOGS);
    if (iFile.is_open() && !iFile.eof()) {
        syslog(LOG_ERR, "ifstream opened");
        pid_t p;
        iFile >> p;
        if (p > 0)
            kill(p, SIGTERM);
    }
    iFile.close();

    std::ofstream oFile(PID_LOGS);
    if (!oFile) {
        syslog(LOG_ERR, "Could not open pid logs");
        exit(EXIT_FAILURE);
    }
    oFile << getpid();
    oFile.close();
}

void Daemon::exec() {
    while (true) {
        fd.exec();
        sleep(wait_sec);
    }
}


void Daemon::update() {
    config.read();
    fd.setPar(config.getDirPath());
    wait_sec = config.getWaitTime();
}

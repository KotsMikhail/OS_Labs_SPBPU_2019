#include <iostream>
#include <syslog.h>
#include <signal.h>
#include <sys/inotify.h>
#include "core/inotify.hpp"
#include "core/ConfigHolder.h"
#include <unistd.h>
#include <sys/stat.h>
#include "core/utils.hpp"

using namespace std;
#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))

static std::string cfgFile = "inotify.cfg";

static inotifier* inotifier;

int close(int ret_code = 0)
{
    delete inotifier;
    closelog();
    ConfigHolder::destroy();
    exit(ret_code);
}

void signalHandler(int sig)
{
    int rc = 0;
    switch (sig){
        case SIGHUP:
            syslog(LOG_LOCAL0, "reload config signal was accepted");
            //reread config file
            rc = ConfigHolder::init(cfgFile);
            if (rc) {
                syslog(LOG_LOCAL0, "can't reload conf file, try again");
                break;
            }

            try {
                inotifier->reloadNotifier();
                inotifier->printWatchDirs();
                syslog(LOG_LOCAL0, "config was successfully reloaded");
            }
            catch (CommonException& e)
            {
                syslog(LOG_LOCAL0, "%s, %s", "can't reload notifier, error is: ",  e.what());
            }

            break;
        case SIGTERM:
            //print to syslog
            syslog (LOG_LOCAL0, "Program terminated");;
            close();
            break;
    }
}

int main(int argc, char** argv) {
    if (argc < 2)
    {
        cout << "use config path: /lab1/path/to/inotify.cfg";
        return (EXIT_FAILURE);
    }

    //init conf file
    cfgFile = argv[1];
    if (ConfigHolder::init(cfgFile))
    {
        cout << "can't open config file: " << argv[1];
        return (EXIT_FAILURE);
    }

    /* Open the log file */
    openlog ("inotify ", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    syslog(LOG_LOCAL0, "Starting daemon...");

    int pid = fork();
    if (pid == -1)
    {
        cout << "can't start daemon";
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
        //set signals
        signal(SIGTERM, signalHandler);
        signal(SIGHUP, signalHandler);

        int ec = setsid();
        if (ec == -1)
            throw CommonException("can't set sid");

        pid = fork();
        if (pid == -1)
        {
            syslog(LOG_ERR, "can't fork second time");
            close();
        }
        if (pid == 0)
        {
            //check daemon already exist
            utils::pidFile::updatePidFile();
            cout << "Hello from child with pid = " << getpid() << " and ppid = " << getppid() << endl;
            //child
            umask(0);
            ec = chdir("/");
            if (ec == -1)
                throw CommonException("can't chdir to /");
            try{
                inotifier = inotifier::createNotifier();
                inotifier->runNotifier();
            }
            catch (CommonException& e)
            {
                syslog(LOG_LOCAL0, "%s", e.what());
                close(1);
            }
        }
        //parent exit
        syslog(LOG_LOCAL0, "parent exit...");
    }
    else //it is parent
    {
        cout << "parent here";
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
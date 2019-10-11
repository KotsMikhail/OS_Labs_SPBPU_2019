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


int close(int ret_code = 0)
{
    closelog();
    ConfigHolder::destroy();
    exit(ret_code);
}

static inotifier* inotifier;

void signalHandler(int sig)
{
    int rc = 0;
    switch (sig){
        case SIGHUP:
            //reread config file
            rc = ConfigHolder::init(cfgFile);
            if (rc) {
                syslog(LOG_LOCAL0, "can't reload conf file, try again");
                break;
            }

            try {
                inotifier->reloadNotifier();
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
    openlog ("inotify ", 0, LOG_LOCAL0);
    syslog(LOG_LOCAL0, "Starting daemon...");

    try{
        inotifier = inotifier::createNotifier();
        inotifier->runNotifier();
    }
    catch (CommonException& e)
    {
        syslog(LOG_LOCAL0, "%s", e.what());
        delete inotifier;
        return EXIT_FAILURE;
    }
/*
    int pid = fork();
    if (pid == -1)
    {
        cout << "can't start daemon";
        return EXIT_FAILURE;
    }
    else if (pid == 0)
    {
        //child started
        //this code executes inside child

        //set files rules bits
        umask(0);

        //create new session independent from parent
        setsid();

        //close input/output descriptots
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        //set signals
        signal(SIGTERM, signalHandler);
        signal(SIGHUP, signalHandler);

        openlog ("inotify ", 0, LOG_LOCAL0);
        syslog(LOG_LOCAL0, "Starting daemon...");

        try{
            inotifier = inotifier::createNotifier();
            inotifier->runNotifier();
        }
        catch (CommonException& e)
        {
            syslog(LOG_LOCAL0, "%s", e.what());
            delete inotifier;
            return EXIT_FAILURE;
        }
    }
    else //it is parent
    {
        return EXIT_SUCCESS;
    }*/

    return EXIT_SUCCESS;
}
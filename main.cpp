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

void signalHandler(int sig)
{
    switch (sig){
        case SIGHUP:
            //reread config file
            ConfigHolder::init(cfgFile);
            break;
        case SIGTERM:
            //print to syslog
            syslog (LOG_LOCAL0, "Program terminated");;
            close();
    }
}

static void skeleton_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();
    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    /* Catch, ignore and handle signals */
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
}

int main() {
    try{

        /* Open the log file */
        openlog ("inotify lab", 0, LOG_LOCAL0);

        //create daemon
        skeleton_daemon();
        syslog(LOG_LOCAL0, "inotify started");

        //init config holder
        cfgFile = utils::getCurrentPath() + "/" + cfgFile;
        ConfigHolder::init(cfgFile);

        //init inotify
        int fd = inotify_init();
        if ( fd < 0 ) {
            syslog(LOG_ERR, "can't init inotify");
            close(-1);
        }
        //add watcher for directory
        map<int, string> wd_map = inotify::addWatchers(fd);

        char *buf = new char[BUF_LEN];

        while (true)
        {
            int offset = 0;
            int len = read(fd, buf, BUF_LEN);
            if (len == -1)
            {
                syslog(LOG_ERR, "can't read buff from fd");
                close(-1);
            }

            auto *event = ( inotify_event * ) &buf[ offset ];

            while (offset < len) {
                if (event->len) {
                    if (event->mask & IN_CREATE) {
                        if (event->mask & IN_ISDIR)
                            syslog(LOG_LOCAL0, "The directory %s was Created into watch_dir %s\n", event->name, wd_map.find(event->wd)->second.c_str());
                        else{
                            syslog(LOG_LOCAL0, "The file %s was Created into watch_dir %s\n", event->name,  wd_map.find(event->wd)->second.c_str());
                        }
                    }

                    if (event->mask & IN_MODIFY) {
                        if (event->mask & IN_ISDIR)
                            syslog(LOG_LOCAL0, "The directory %s was modified into watch_dir %s\n", event->name, wd_map.find(event->wd)->second.c_str());
                        else
                            syslog(LOG_LOCAL0, "The file %s was modified into watch_dir %s\n", event->name, wd_map.find(event->wd)->second.c_str());
                    }

                    if (event->mask & IN_DELETE) {
                        if (event->mask & IN_ISDIR)
                            syslog(LOG_LOCAL0, "The directory %s was deleted into watch_dir %s\n", event->name,  wd_map.find(event->wd)->second.c_str());
                        else
                            syslog(LOG_LOCAL0, "The file %s was deleted into watch_dir %s\n", event->name, wd_map.find(event->wd)->second.c_str());
                    }

                    offset += EVENT_SIZE * event->len;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        syslog(LOG_ERR, "exception was catched");
        close(-1);
    }
    close(0);
    return 0;
}
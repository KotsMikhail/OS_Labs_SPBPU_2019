#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "inotify.h"
#include "config.h"
#include "utils.h"

#define PID_FILE "/var/run/disk_monitor.pid"

void load_config()
{
    config_t * config = config_t::get_instance();
    inotify_t * inotify = inotify_t::get_instance();
    if (!config || !inotify)
        return;
    
    config->load();
    inotify->add_watchers(config->get_difference_add());
    inotify->remove_watchers(config->get_difference_delete());
}

void signal_handler(int sig) 
{
    switch (sig) 
    {
    case SIGHUP:
        syslog(LOG_NOTICE, "Hangup signal catched.");
        load_config();
        break;
    case SIGTERM:
        syslog(LOG_NOTICE, "Terminate signal catched. Stopping disk_monitor.");
        inotify_t::destroy();
        config_t::destroy();
        exit(EXIT_SUCCESS);
        break;
    }
}

void terminate_another_instance() 
{
    pid_t prev;
    string_t proc("/proc/");
    ifstream_t pid_file(PID_FILE);

    if (pid_file.is_open() && !pid_file.eof())
    {
        pid_file >> prev;
        proc += std::to_string(prev);
        if (is_dir(proc))
            kill(prev, SIGTERM); 
    }
    pid_file.close();
}

void save_pid() 
{
    ofstream_t pid_file(PID_FILE);
    if (pid_file.is_open())
    {
        pid_file << getpid();  
        pid_file.close();
    }
}

void init_base()
{
    umask(0);

    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Couldn't generate session ID for child process.");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Couldn't change working directory to /.");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
}

int main(int argc, char **argv) 
{
    config_t * config;
    inotify_t * inotify;
    pid_t pid;

    pid = fork();
    if (pid == -1) 
        exit(EXIT_FAILURE);
    else if (pid > 0) 
        exit(EXIT_SUCCESS); 

    try
    {
        terminate_another_instance();
        openlog("disk_monitor", LOG_NOWAIT | LOG_PID, LOG_LOCAL0);
        syslog(LOG_NOTICE, "Started disk_monitor.");  

        initialize_home_directory();
        if (argc > 1)
            config = config_t::get_instance(argv[1]);
        if (!config)
        {
            syslog(LOG_ERR, "Couldn't initialize configuration file. Stopped disk_monitor.");
            closelog();
            exit(EXIT_FAILURE);
        }
        syslog(LOG_NOTICE, "Successfully initialized configuration file.");  

        inotify = inotify_t::get_instance();
        if (!inotify)
        {
            syslog(LOG_WARNING, "Couldn't initialize inotify. Stopped disk_monitor.");
            closelog();
            exit(EXIT_FAILURE);
        }
        syslog(LOG_NOTICE, "Successfully initialized inotify.");
        
        save_pid();
        init_base();
        load_config();

        while (true)
            inotify->do_inotify();
    }
    catch (const std::exception &e)
    {
        syslog(LOG_ERR, "%s.", e.what());
        syslog(LOG_NOTICE, "Stopping disk_monitor.");
        closelog();
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

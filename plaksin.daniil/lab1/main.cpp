#include "daemon.h"

int main(int argc,char **argv)
{
    int interval = 10;

    pid_t pid = fork();

    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    if (argc < 2)
    {
        printf("Wrong numbers of arguments. Expected: 2. Got: %d\n", argc);
        exit(EXIT_FAILURE);
    }
    
    Daemon::get_instance().cfg_path = argv[1];

    openlog("daemon_lab", LOG_NOWAIT | LOG_PID, LOG_USER);

    umask(0);

    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Could not generate session ID for child process");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
        exit(EXIT_FAILURE);
    else if (pid > 0)
        exit(EXIT_SUCCESS);
    
    Daemon::get_instance().cfg_path = realpath(Daemon::get_instance().cfg_path.c_str(), nullptr);

    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Could not change working directory to /");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Successfully started daemon_lab");
    //std::cout << "pid: " << pid << std::endl;
    close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, Daemon::signal_handler);
    signal(SIGTERM, Daemon::signal_handler);

    Daemon::get_instance().kill_prev_daemon();

    Daemon::get_instance().set_pid_file();
    Daemon::get_instance().read_config();
    while (true)
    {
        //std::cout << "work" << std::endl;
        Daemon::get_instance().process_config_file(interval);
        
        sleep(interval);
    }
}
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
    Daemon* daemon = Daemon::get_instance();
    daemon->cfg_path = argv[1];

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
    
    daemon->cfg_path = realpath(daemon->cfg_path.c_str(), nullptr);

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

    daemon->kill_prev_daemon();

    daemon->set_pid_file();
    daemon->read_config();
    while (true)
    {
        //std::cout << "work" << std::endl;
        daemon->process_config_file(interval);
        
        sleep(interval);
    }
}
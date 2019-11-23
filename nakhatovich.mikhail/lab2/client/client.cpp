#include <sys/stat.h>
#include <syslog.h>

#include <string>

#include "user_client.h"

int main(int argc, char* argv[])
{
    openlog("lab2", LOG_NOWAIT | LOG_PID, LOG_LOCAL1);
    syslog(LOG_NOTICE, "client: started.");

    if (argc < 2)
    {
        syslog(LOG_ERR, "client: couldn't find pid of host.");
        closelog();
        return EXIT_FAILURE;
    }
    
    int host_pid;
    try
    {
        host_pid = std::stoi(argv[1]);
    }
    catch (std::exception &e)
    {
        syslog(LOG_ERR, "client: couldn't get int pid from argument.");
        closelog();
        return EXIT_FAILURE;
    }

    std::string proc("/proc/" + std::to_string(host_pid));
    struct stat s;
    if (stat(proc.c_str(), &s) || !S_ISDIR(s.st_mode))
    {
        syslog(LOG_ERR, "client: couldn't find process with pid %d.", host_pid);
        closelog();
        return EXIT_FAILURE;
    }

    client_t &client = client_t::get_instance(host_pid);
    if (client.open_connection())
        client.run();

    syslog(LOG_NOTICE, "client: stopped.");
    closelog();
    return EXIT_SUCCESS;
}

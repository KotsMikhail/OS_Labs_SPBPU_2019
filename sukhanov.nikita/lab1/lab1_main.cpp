
#include "Daemon.h"

void SignalHandler(int signal)
{
    switch (signal)
    {
    case SIGHUP:
        syslog(LOG_NOTICE, "Re-reading config");
        Daemon::GetDaemon().ReadConfig();
        break;
    case SIGTERM:
        syslog(LOG_NOTICE, "Terminating");
        unlink(Daemon::PID_PATH.c_str());
        exit(EXIT_SUCCESS);
        break;
    default:
        syslog(LOG_NOTICE, "Unknown signal %d", signal);
        break;
    }
}

int main(int argc, char *argv[])
{
    Daemon &daemon = Daemon::GetDaemon();
    openlog("daemon_lab1", LOG_PID, LOG_DAEMON);
    if (argc < 2)
    {
        syslog(LOG_ERR, "Not enough arguments");
        exit(EXIT_FAILURE);
    }
    daemon.SetConfigPath(argv[1]);
    Daemon::Daemonize(SignalHandler);
    daemon.Run();
}
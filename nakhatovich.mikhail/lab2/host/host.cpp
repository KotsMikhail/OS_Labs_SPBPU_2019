#include <syslog.h>
#include <unistd.h>

#include "user_host.h"

int main()
{
    openlog("lab2", LOG_NOWAIT | LOG_PID, LOG_LOCAL1);
    syslog(LOG_NOTICE, "host: started with pid %d.", getpid());

    host_t & host = host_t::get_instance();
    if (host.open_connection())
        host.run();
    else
    {
        syslog(LOG_NOTICE, "host: stopped.");
        closelog();
    }
    
    return EXIT_SUCCESS;
}

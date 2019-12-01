#include "host.h"
#include <syslog.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

int main() {
    openlog ("host", LOG_PID, LOG_LOCAL0);

    host_t &host = host_t::get_instance();

    std::cout << "Host (pid = " << getpid() << ") awoke!" << std::endl;
    syslog(LOG_NOTICE, "Host awoke!");

    if (host.connect() != 0) {
        std::cout << "Error: connection failed" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Host: connected successfully" << std::endl;

    host.run();

    closelog();

    return EXIT_SUCCESS;
}
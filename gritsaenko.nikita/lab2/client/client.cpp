#include "client.h"
#include <syslog.h>
#include <sys/stat.h>
#include <iostream>

int main(int argc, char* argv[]) {
    openlog("client", LOG_PID, LOG_LOCAL0);

    if (argc < 2) {
        std::cout << "Error: no host pid selected" << std::endl;
        syslog(LOG_ERR, "Error: no host pid selected");
        closelog();
        return EXIT_FAILURE;
    }
    
    int host_pid = std::stoi(argv[1]);

    std::string proc = "/proc/" + std::to_string(host_pid);

    struct stat s;

    if (stat(proc.c_str(), &s) || !S_ISDIR(s.st_mode)) {
        std::cout << "Error: Invalid host pid" << std::endl;
        syslog(LOG_ERR, "Error: host with pid %d doesn't exist!", host_pid);
        closelog();
        return EXIT_FAILURE;
    }

    client_t &client = client_t::get_instance(host_pid);

    std::cout << "Client awoke!" << std::endl;
    syslog(LOG_NOTICE, "Client awoke");

    client.connect();
    
    client.run();  

    std::cout << "Client finished" << std::endl;
    syslog(LOG_NOTICE, "Client finished");


    closelog();

    return EXIT_SUCCESS;
}
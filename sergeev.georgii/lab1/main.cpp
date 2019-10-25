#include <string>
#include "Daemon.h"
#include <sys/syslog.h>

int main(int argc, char **argv) {
    std::string config_file_path;
    if (argc < 2) {
        printf("Error: expected 2 arguments\n");
        return EXIT_FAILURE;
    }
    std::string config_file_name = argv[1];
    if (Daemon::init(config_file_name) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    Daemon::run();

    syslog(LOG_NOTICE, "Close syslog");
    closelog();
    return EXIT_SUCCESS;
}
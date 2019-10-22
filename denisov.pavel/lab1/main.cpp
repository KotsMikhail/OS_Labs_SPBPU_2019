#include <iostream>
#include <unistd.h>
#include <syslog.h>

#include "bk_files_copy_daemon.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Bad argmunents amount. Usage: ./lab1 /rel_path/to/config/file" << std::endl;
        return -1;
    }

    std::string configFileRelPath(argv[1]);
    if (!LoadConfig(std::string(get_current_dir_name()) + "/" + configFileRelPath)) {
        std::cout << "Error! Failed to parse config file." << std::endl;
        return -1;
    }

    openlog("bk_files_copier_daemon", LOG_NOWAIT | LOG_PID, LOG_LOCAL0);
    Daemonise();
    DaemonWorkLoop();

    return 0;
}
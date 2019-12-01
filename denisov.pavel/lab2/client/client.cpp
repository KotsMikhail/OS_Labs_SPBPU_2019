#include <iostream>

#include "client.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "[ERROR]: Usage: ./client_<connection_type> <host_pid>" << std::endl;
        return 1;
    }

    pid_t hostPid;
    try {
        hostPid = std::stoi(argv[1]);
    } catch (std::exception &e) {
        std::cout << "[ERROR]: Can't parse host's pid from argument" << std::endl;
        return 1;
    }

    std::cout << "Starting client..." << std::endl;
    Client& cl = Client::GetInstance(hostPid);
    if (cl.OpenConnection()) {
        cl.Start();
    }
    return 0;
}
//
// Created by dmitrii on 25.11.2019.
//

#include "client_impl.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Run by: ./client_<connection_type> <host pid>" << std::endl;
        return 1;
    }
    int pid;
    try
    {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR: can't convert to int pid argument" << std::endl;
        return -1;
    }
    Client& client = Client::getInstance(pid);
    if (client.openConnection())
    {
        client.run();
    }
    return 0;
}

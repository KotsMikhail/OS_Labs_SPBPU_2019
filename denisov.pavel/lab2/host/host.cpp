#include <iostream>

#include "host.h"

int main()
{
    std::cout << "Starting the host..." << std::endl;
    Host& host = Host::GetInstance();
    if (host.OpenConnection()) {
        host.Start();
    }
    return 0;
}
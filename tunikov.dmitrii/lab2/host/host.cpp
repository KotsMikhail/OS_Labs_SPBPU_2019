//
// Created by dmitrii on 25.11.2019.
//
#include <iostream>
#include "host_impl.h"

int main()
{
    std::cout << "Host starting.." << std::endl;

    Host& h = Host::getInstance();
    if (h.openConnection())
    {
        h.run();
    }
    return 0;
}
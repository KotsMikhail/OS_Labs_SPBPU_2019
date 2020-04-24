#include <iostream>
#include <cstdlib>
#include "wolf.h"

int main(int argc, char**argv) {
    std::cout << "Starting host..." << std::endl;
    std::cout << "pid: " << getpid() << std::endl;

    if (argc != 2) {
        std::cout << "Expected number of goats";
        return 0;
    }

    int n = std::atoi(argv[1]);
    if (n <= 0) {
        std::cout << "Number of goats must be >= 0";
        return 0;
    }

    Wolf &wolf = Wolf::GetInstance(n);
    if (wolf.OpenConnection()) {
        wolf.Start();
    }

    return 0;
}

#include <iostream>

#include "config.h"
#include "support.h"
#include "test.h"


int main (int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Usage: ./Lab3_set <path_to_config>" << std::endl;
        exit(EXIT_FAILURE);
    }

    initHomeDir();
    CONFIG *config = CONFIG::GetInstance(argv[1]);
    if (!config->Load()) {
        std::cout << "Failed config file initialization." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "----------------- Fine-Grained Set TEST -----------------" << std::endl;
    runTest(TESTING_SETS::FINE_GRAINED, TEST_TYPE::WRITERS_TEST);
    runTest(TESTING_SETS::FINE_GRAINED, TEST_TYPE::READERS_TEST);
    runTest(TESTING_SETS::FINE_GRAINED, TEST_TYPE::GENERAL_TEST);

    std::cout << "----------------- Lazy-Synchronization Set TEST -----------------" << std::endl;
    runTest(TESTING_SETS::LAZY_SYNCHRONIZATION, TEST_TYPE::WRITERS_TEST);
    runTest(TESTING_SETS::LAZY_SYNCHRONIZATION, TEST_TYPE::READERS_TEST);
    runTest(TESTING_SETS::LAZY_SYNCHRONIZATION, TEST_TYPE::GENERAL_TEST);

    std::cout << "----------------- TIME TEST -----------------" << std::endl;
    runTimeTest();
}
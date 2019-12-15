#include <iostream>
#include "tests/TestRunner.h"
#include <thread>


int main(int argc, char** argv)
{
    if (argc < 5)
    {
        std::cout << "Wrong program arguments. Program has 4 mandatory artuments: writers_count, write_action_count, readers_count, read_action_count" << std::endl;
        return -1;
    }

    FullTestParams test_params{};
    try {
        test_params.writer_params = {std::stoi(argv[1]), std::stoi(argv[2])};
        test_params.reader_params = {std::stoi(argv[3]), std::stoi(argv[4])};
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Can't parse integer program arguments" << std::endl;
        return -1;
    }


    int threads_max_count = (int)std::thread::hardware_concurrency();
    if (test_params.reader_params.workers_count > threads_max_count || test_params.writer_params.workers_count > threads_max_count)
    {
        std::cout << "wrong writers/readers count, must be <= " << threads_max_count << std::endl;
        return -1;
    }

    TestRunner test_runner = TestRunner(test_params);
    test_runner.runTests();

    return 0;
}

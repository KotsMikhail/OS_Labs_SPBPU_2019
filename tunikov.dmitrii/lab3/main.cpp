#include <iostream>
#include "tests/TestRunner.h"
#include "utils/utils.h"
#include <stdexcept>

FullTestParams parseProgramArguments(char** argv)
{
    FullTestParams test_params{};

    test_params.writer_params = {std::stoi(argv[1]), std::stoi(argv[2])};
    test_params.reader_params = {std::stoi(argv[3]), std::stoi(argv[4])};

    int max_threads_count = utils::getMaxThreadsCount();
    if (test_params.reader_params.workers_count > max_threads_count || test_params.writer_params.workers_count > max_threads_count)
        throw std::runtime_error(std::string( "wrong writers/readers count, must be <= ") + std::to_string(max_threads_count));

    return test_params;
}

int main(int argc, char** argv)
{
    if (argc < 5)
    {
        std::cout << "Wrong program arguments. Program has 4 mandatory artuments: writers_count, write_action_count, readers_count, read_action_count" << std::endl;
        return -1;
    }

    try {
        FullTestParams test_params = parseProgramArguments(argv);

        TestRunner test_runner = TestRunner(test_params);
        test_runner.runTests();
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Can't parse integer program arguments" << std::endl;
        return -1;
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }


    return 0;
}

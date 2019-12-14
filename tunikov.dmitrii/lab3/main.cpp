#include <iostream>
#include "blocking_stack/BlockingStack.h"
#include "tests/TestRunner.h"

int main(int argc, char** argv)
{
    if (argc < 5)
    {
        std::cout << "Wrong program arguments. Program has 4 mandatory artuments: writers_count, write_action_count, readers_count, read_action_count" << std::endl;
        return -1;
    }

    TestRunnerParams test_params{};
    try {
        test_params.writer_params = {std::stoi(argv[1]), std::stoi(argv[2])};
        test_params.reader_params = {std::stoi(argv[3]), std::stoi(argv[4])};
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Can't parse integer program arguments" << std::endl;
        return -1;
    }

    TestRunner test_runner = TestRunner(test_params);
    test_runner.runTests();

    return 0;
}

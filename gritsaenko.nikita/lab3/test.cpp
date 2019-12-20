#include <stdio.h>

#include "lock_free_stack.hpp"
#include "lock_stack.hpp"
#include "tester.hpp"
#include <iostream>

static int N = 0;
static int N_MAX_THREADS = 0;
static const int NUM_OF_REPEATS = 30;
static const int NUM_OF_PARTS = 3;

template <typename T>
tester_t<T> * tester_t<T>::instance = NULL;

void test_readers()
{
    int step = N_MAX_THREADS / NUM_OF_PARTS;

    for (int i = 1; i <= N_MAX_THREADS; i += step)
    {
        
        int time_lock = 0;
        int time_free_lock = 0;

        for (int j = 1; j <= NUM_OF_REPEATS; j++)
        {
            tester_t<lock_stack_t<int>> lock_tester(N, 0, i);

            time_lock += lock_tester.test_readers();

            tester_t<lock_free_stack_t<int>> lock_free_tester(N, 0, i);

            time_free_lock += lock_free_tester.test_readers();
        }

        std::cout << "Test readers.      Lock Stack number of threads:       " << i << "      ";
        std::cout << "Average time: " << time_lock / NUM_OF_REPEATS << std::endl; 

        std::cout << "Test readers.      Lock Free Stack number of threads:  " << i << "      ";
        std::cout << "Average time: " << time_free_lock / NUM_OF_REPEATS << std::endl; 
    
    }

    std::cout << std::endl << std::endl;
}

void test_writers()
{
    int step = N_MAX_THREADS / NUM_OF_PARTS;

    for (int i = 1; i <= N_MAX_THREADS; i += step)
    {
        
        int time_lock = 0;
        int time_free_lock = 0;

        for (int j = 1; j <= NUM_OF_REPEATS; j++)
        {
            tester_t<lock_stack_t<int>> lock_tester(N, i, 0);

            time_lock += lock_tester.test_writers();

            tester_t<lock_free_stack_t<int>> lock_free_tester(N, i, 0);

            time_free_lock += lock_free_tester.test_writers();
        }

        std::cout << "Test writers.      Lock Stack number of threads:       " << i << "      ";
        std::cout << "Average time: " << time_lock / NUM_OF_REPEATS << std::endl; 

        std::cout << "Test writers.      Lock Free Stack number of threads:  " << i << "      ";
        std::cout << "Average time: " << time_free_lock / NUM_OF_REPEATS << std::endl; 
    
    }

    std::cout << std::endl << std::endl;
}

void test_both()
{
    int step = N_MAX_THREADS / NUM_OF_PARTS;

    for (int i = 1; i <= N_MAX_THREADS; i += step)
    {
        for (int j = 1; j <= N_MAX_THREADS; j += step)
        {
            int time_lock_read = 0, time_lock_write = 0;
            int time_free_lock_read = 0, time_free_lock_write = 0;

            for (int k = 1; k <= NUM_OF_REPEATS; k++)
            {
                tester_t<lock_stack_t<int>> lock_tester(N, i, j);

                std::pair<int, int> time = lock_tester.test_readers_and_writers();   
                time_lock_write += time.first;
                time_lock_read += time.second;

                tester_t<lock_free_stack_t<int>> lock_free_tester(N, i, j);

                time = lock_free_tester.test_readers_and_writers();
                time_free_lock_write += time.first;
                time_free_lock_read += time.second;
            }

            std::cout << "Test both.     Lock Stack.            Writers: " << i << "  Readers: " << j << "        ";
            std::cout << "Write time: " << time_lock_write / NUM_OF_REPEATS << "       ";
            std::cout << "Read time: " << time_lock_read / NUM_OF_REPEATS; 
            std::cout << std::endl;
            
            std::cout << "Test both.     Lock Free Stack.       Writers: " << i << "  Readers: " << j << "        ";
            std::cout << "Write time: " << time_free_lock_write / NUM_OF_REPEATS << "       ";
            std::cout << "Read time: " << time_free_lock_read / NUM_OF_REPEATS; 
            std::cout << std::endl;
        }
        
    }

    std::cout << std::endl << std::endl;
}

int main (int argc, char ** argv)
{
    std::string input;

    std::cout << "Input maximum number of threads: ";
    std::cin >> input;
    std::cout << std::endl;

    try
    {
        N_MAX_THREADS = std::stoi(input);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: incorrect input." << std::endl;
        return EXIT_FAILURE;
    }

    if (N_MAX_THREADS < 0)
    {
        std::cout << "Error: incorrect input (number of threads < 0)" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Input size of array: ";
    std::cin >> input;
    std::cout << std::endl;

    try
    {
        N = std::stoi(input);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: incorrect input." << std::endl;
        return EXIT_FAILURE;
    }

    if (N < 0)
    {
        std::cout << "Error: incorrect input (array size < 0)" << std::endl;
        return EXIT_FAILURE;
    }

    openlog("tester", LOG_PID, LOG_LOCAL0);

    test_readers();
    test_writers();
    test_both();

    closelog();

    return EXIT_SUCCESS;
}

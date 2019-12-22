#include <stdio.h>

#include "lock_free_stack.hpp"
#include "lock_stack.hpp"
#include "tester.hpp"
#include <iostream>

static int N_READERS = 0;
static int N_WRITERS = 0;
static int ARRAY_SIZE = 0;

static const int NUM_OF_REPEATS = 10;

template <typename T>
tester_t<T> * tester_t<T>::instance = NULL;

void test_readers()
{
    int time_lock = 0;
    int time_free_lock = 0;

    for (int j = 1; j <= NUM_OF_REPEATS; j++)
    {
        tester_t<lock_stack_t<int>> lock_tester(ARRAY_SIZE, 0, N_READERS);
        time_lock += lock_tester.test_readers();

        tester_t<lock_free_stack_t<int>> lock_free_tester(ARRAY_SIZE, 0, N_READERS);
        time_free_lock += lock_free_tester.test_readers();
    }

    std::cout << "Test readers.      Lock Stack number of threads:       " << N_READERS << "      ";
    std::cout << "Average time: " << time_lock / NUM_OF_REPEATS << std::endl; 

    std::cout << "Test readers.      Lock Free Stack number of threads:  " << N_READERS << "      ";
    std::cout << "Average time: " << time_free_lock / NUM_OF_REPEATS << std::endl; 
    
    std::cout << std::endl << std::endl;
}

void test_writers()
{
    int time_lock = 0;
    int time_free_lock = 0;

    for (int j = 1; j <= NUM_OF_REPEATS; j++)
    {
        tester_t<lock_stack_t<int>> lock_tester(ARRAY_SIZE, N_WRITERS, 0);
        time_lock += lock_tester.test_writers();

        tester_t<lock_free_stack_t<int>> lock_free_tester(ARRAY_SIZE, N_WRITERS, 0);
        time_free_lock += lock_free_tester.test_writers();
    }

    std::cout << "Test writers.      Lock Stack number of threads:       " << N_WRITERS << "      ";
    std::cout << "Average time: " << time_lock / NUM_OF_REPEATS << std::endl; 

    std::cout << "Test writers.      Lock Free Stack number of threads:  " << N_WRITERS << "      ";
    std::cout << "Average time: " << time_free_lock / NUM_OF_REPEATS << std::endl; 
    
    std::cout << std::endl << std::endl;
}

void test_both()
{
    int writers_combinations = 10;
    int readers_combinations = 10;
    int tests_total = writers_combinations * readers_combinations;

    int step_readers = N_READERS / readers_combinations;
    int step_writers = N_WRITERS / writers_combinations;

    int time_lock_read = 0, time_lock_write = 0;
    int time_free_lock_read = 0, time_free_lock_write = 0;

    for (int i = 1; i <= N_WRITERS; i += step_writers)
    {
        for (int j = 1; j <= N_READERS; j += step_readers)
        {
            tester_t<lock_stack_t<int>> lock_tester(ARRAY_SIZE, i, j);

            std::pair<int, int> time = lock_tester.test_readers_and_writers();   
            time_lock_write += time.first;
            time_lock_read += time.second;

            tester_t<lock_free_stack_t<int>> lock_free_tester(ARRAY_SIZE, i, j);

            time = lock_free_tester.test_readers_and_writers();
            time_free_lock_write += time.first;
            time_free_lock_read += time.second;
        }
        
    }

    std::cout << "Test both.     Lock Stack.         Writers max: " << N_WRITERS << "  Readers max: " << N_READERS << "     ";
    std::cout << "Write time average: " << time_lock_write / tests_total << "    ";
    std::cout << "Read time average: " << time_lock_read / tests_total; 
    std::cout << std::endl;
            
    std::cout << "Test both.     Lock Free Stack.    Writers max: " << N_WRITERS << "  Readers max: " << N_READERS << "     ";
    std::cout << "Write time average: " << time_free_lock_write / tests_total << "    ";
    std::cout << "Read time average: " << time_free_lock_read / tests_total; 
    std::cout << std::endl;

}


bool process_input()
{
    std::string input;

    std::cout << "Input number of readers: ";
    std::cin >> input;
    std::cout << std::endl;

    try
    {
        N_READERS = std::stoi(input);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: incorrect input." << std::endl;
        return false;

    }

    if (N_READERS <= 0)
    {
        std::cout << "Error: incorrect input (number of readers <= 0)" << std::endl;
        return false;

    }

    std::cout << "Input number of writers: ";
    std::cin >> input;
    std::cout << std::endl;

    try
    {
        N_WRITERS = std::stoi(input);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: incorrect input." << std::endl;
        return false;
    }

    if (N_WRITERS <= 0)
    {
        std::cout << "Error: incorrect input (number of writers <= 0)" << std::endl;
        return false;
    }

    std::cout << "Input size of array: ";
    std::cin >> input;
    std::cout << std::endl;

    try
    {
        ARRAY_SIZE = std::stoi(input);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: incorrect input." << std::endl;
        return false;
    }

    if (ARRAY_SIZE <= 0)
    {
        std::cout << "Error: incorrect input (array size <= 0)" << std::endl;
        return false;
    }

    return true;
}

int main (int argc, char ** argv)
{
    if (!process_input())
    {
        return EXIT_FAILURE;
    }

    openlog("tester", LOG_PID, LOG_LOCAL0);

    std::cout << "Testing... Please wait." << std::endl << std::endl;

    test_readers();
    test_writers();
    test_both();

    closelog();

    return EXIT_SUCCESS;
}

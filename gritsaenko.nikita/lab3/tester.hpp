#ifndef TIME_TESTER
#define TIME_TESTER

#include <time.h>
#include <iostream>
#include "utils.hpp"
#include "stack.hpp"
#include <pthread.h>
#include <syslog.h>
#include <assert.h>

template <typename T>
class tester_t
{
public:
	tester_t(int array_size, int n_writers = 0, int n_readers = 0): stack()
	{
		this->n_writers = n_writers;
		this->array_size = array_size;
		this->n_readers = n_readers;
		array = new int[array_size];
		instance = this;
	}

	~tester_t()
	{
		if (array)
		{
			delete[] array;
		}
	}

	int test_readers()
	{
		int* tmp = allocate_filled_array(array_size, 0);

		if (tmp == NULL) 
		{
			return 0;
		}
		instance->array = tmp;
		fill_stack(stack, array_size);
		pthread_t readers[n_readers];

		int reader_time = 0;
		
		for (int i = 0; i < n_readers; ++i)
		{
			if (pthread_create(&readers[i], NULL, tester_t::thread_read, new int(i)) != 0)
			{
				syslog(LOG_ERR, "Error: pthread_create failed (test_readers)");
				return 0;
			}
		}

		reader_time = clock();

		for (int i = 0; i < n_readers; ++i)
		{
			if (pthread_join(readers[i], NULL) != 0)
			{
				syslog(LOG_ERR, "Error: pthread_join failed (test_readers)");
				return 0;
			}
		}

		reader_time = clock() - reader_time;

		assert(check_array_values(instance->array, array_size, 1));
		
		assert(stack.empty());

		return reader_time;
	}

	int test_writers()
	{
		int* tmp = allocate_filled_array(array_size, 0);

		if (tmp == NULL) 
		{
			return 0;
		}
		instance->array = tmp;
		
		pthread_t writers[n_writers];

		int writer_time = 0;
		
		for (int i = 0; i < n_writers; ++i)
		{
			if (pthread_create(&writers[i], NULL, tester_t::thread_write, new int(i)) != 0)
			{
				syslog(LOG_ERR, "Error: pthread_create failed (test_writers)");
				return 0;
			}
		}

		writer_time = clock();

		for (int i = 0; i < n_writers; ++i)
		{
			if (pthread_join(writers[i], NULL) != 0)
			{
				syslog(LOG_ERR, "Error: pthread_join failed (test_writers)");
				return 0;
			}
		}

		writer_time = clock() - writer_time;

		int *stack_values = array_from_stack(instance->stack, array_size);

		assert(check_array_values(stack_values, array_size, 1));
		
		delete[] stack_values;

		return writer_time;
	}

	std::pair<int, int> test_readers_and_writers()
	{
		int* tmp = allocate_filled_array(array_size, 0);
		if (tmp == NULL) 
		{
			return std::pair<int, int>(0, 0);
		}

		instance->array = tmp;
		int reader_time = 0, writer_time = 0;

		pthread_t writers[n_writers];
		for (int i = 0; i < n_writers; ++i)
		{
			if (pthread_create(&writers[i], NULL, tester_t::thread_write, new int(i)) != 0) 
			{
				syslog(LOG_ERR, "Error: pthread_create failed (writers)");
				return std::pair<int, int>(0, 0);
			}
		}

		writer_time = clock();

		for (int i = 0; i < n_writers; ++i)
		{
			if (pthread_join(writers[i], NULL) != 0) 
			{
				syslog(LOG_ERR, "Error: pthread_join failed (writers)");
				return std::pair<int, int>(0, 0);
			}
		}

		writer_time = clock() - writer_time;
		
		pthread_t readers[n_readers];
		for (int i = 0; i < n_readers; ++i)
		{
			if (pthread_create(&readers[i], NULL, tester_t::thread_read, new int(i)) != 0) 
			{
				syslog(LOG_ERR, "Error: pthread_create failed (readers)");
				return std::pair<int, int>(0, 0);
			}
		}

		reader_time = clock();

		for (int i = 0; i < n_readers; ++i)
		{
			if (pthread_join(readers[i], NULL) != 0) 
			{
				syslog(LOG_ERR, "Error: pthread_join failed (readers)");
				return std::pair<int, int>(0, 0);
			}
		}	

		reader_time = clock() - reader_time;
		
		assert(check_array_values(instance->array, array_size, 1));
		assert(stack.empty()); 

		return std::pair<int, int>(writer_time, reader_time);
	}

private:
	static void* thread_write(void *idx)
	{
		int writer_idx = *((int*)(idx));
		
		int num_blocks_to_push = instance->array_size / instance->n_writers;


		int start_index = num_blocks_to_push * writer_idx;
		
		if (writer_idx == instance->n_writers - 1) 
		{
			num_blocks_to_push += instance->array_size % instance->n_writers;
		}

		for (int i = start_index; i < start_index + num_blocks_to_push; ++i)
		{
			instance->stack.push(i);
		}

		return NULL;
	}

	static void* thread_read(void *idx)
	{
		std::shared_ptr<int> elem = instance->stack.pop();
		while (elem)
		{
			int i = *elem;
			__atomic_fetch_add(&instance->array[i], 1, __ATOMIC_RELAXED);
			elem = instance->stack.pop();
		}

		return NULL;
	}

private:
	static tester_t *instance;
	T stack;

	int array_size;
	int n_readers;
	int n_writers;
	int *array;
};

#endif

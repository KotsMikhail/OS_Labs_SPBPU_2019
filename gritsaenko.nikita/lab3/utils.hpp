
#ifndef UTILS_H
#define UTILS_H

#include "stack.hpp"
#include <syslog.h>
#include <stdio.h>

int *allocate_filled_array(int size, int value)
{
	int *array = new int[size];

	if (array == NULL)
	{
		syslog(LOG_ERR, "Error: can't allocate memory in method allocate_filled_array");
		return NULL;
	}

	for (int i = 0; i < size; ++i)
	{
		array[i] = value;
	}

	return array;
}

bool check_array_values(int *array, int size, int value)
{
	for (int i = 0; i < size; ++i)
	{
		if (array[i] != value)
		{
			return false;
		}
	}

	return true;
}

template <typename T>
void fill_stack(stack_t<T> &stack, int size)
{
	for (int i = 0; i < size; ++i)
	{
		stack.push(i);
	}
}

template <typename T>
int *array_from_stack(stack_t<T> &stack, int array_size)
{
	int *array = allocate_filled_array(array_size, 0);

	if (array == NULL)
	{
		return NULL;
	}

	while (!stack.empty())
	{
		int i = *stack.pop();
		array[i]++;
	}
	return array;
}

#endif
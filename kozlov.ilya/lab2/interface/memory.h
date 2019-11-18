#ifndef MEMORY_H
#define MEMORY_H

#include "status.h"

struct Memory
{
  Status status;
  int number;

  Memory(Status st = ALIVE, int num = 0) : status(st), number(num)
  {
  }
};

#endif // MEMORY_H
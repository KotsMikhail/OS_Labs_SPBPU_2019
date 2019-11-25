#ifndef GLOBAL_MEMORY_H
#define GLOBAL_MEMORY_H

#include "status.h"

struct Message
{
  Status status;
  int number;

  Message(Status st = Status::ALIVE, int num = 0) : status(st), number(num)
  {
  }
};

#endif // GLOBAL_MEMORY_H

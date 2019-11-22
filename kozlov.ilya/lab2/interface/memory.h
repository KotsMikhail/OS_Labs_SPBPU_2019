#ifndef MEMORY_H
#define MEMORY_H

#include "status.h"

#define SEM_NAME "LAB2"
#define TIMEOUT 5

struct Message
{
  Owner owner;
  Status status;
  int number;

  Message(Owner ow = WOLF, Status st = ALIVE, int num = 0) : owner(ow), status(st), number(num)
  {
  }
};

#endif // MEMORY_H

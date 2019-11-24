#ifndef GLOBAL_MEMORY_H
#define GLOBAL_MEMORY_H

#include "status.h"

#define SEM_HOST_NAME "LAB2_HOST"
#define SEM_CLIENT_NAME "LAB2_CLIENT"
const int TIMEOUT = 5;

struct Message
{
  Status status;
  int number;

  Message(Status st = Status::ALIVE, int num = 0) : status(st), number(num)
  {
  }
};

#endif // GLOBAL_MEMORY_H

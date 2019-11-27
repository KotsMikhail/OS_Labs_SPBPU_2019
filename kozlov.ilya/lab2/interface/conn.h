#ifndef CONN_H
#define CONN_H

#include <cstdlib>
#include <string>

#include "memory.h"

class Conn
{
public:
  bool Open(size_t id, bool create);
  bool Read(void *buf, size_t count = sizeof(Message));
  bool Write(void *buf, size_t count = sizeof(Message));
  bool Close();

private:
  int id;
  bool owner;
  std::string name;
};

#endif // CONN_H

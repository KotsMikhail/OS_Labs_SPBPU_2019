#ifndef CONN_H
#define CONN_H

#include <cstdlib>

class Conn
{
public:
  bool Open(size_t id, bool create);
  bool Read(void *buf, size_t count);
  bool Write(void *buf, size_t count);
  bool Close();
};

#endif // CONN_H

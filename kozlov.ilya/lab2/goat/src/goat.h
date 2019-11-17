#ifndef GOAT_H
#define GOAT_H

#include <conn.h>
#include <bits/types/siginfo_t.h>

class Goat
{
public:
  static Goat GetInstance(int host_pid);
  void Start();
private:
  Conn connection;

  Goat(int host_pid);
  // TODO
};


#endif //GOAT_GOAT_H

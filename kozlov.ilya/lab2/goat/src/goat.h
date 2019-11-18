#ifndef GOAT_H
#define GOAT_H

#include <conn.h>
#include <bits/types/siginfo_t.h>
#include <semaphore.h>

class Goat
{
public:
  static int last_err;

  static Goat GetInstance(int host_pid);
  void Start();
  ~Goat();
private:
  Conn connection;
  sem_t* semaphore;

  Goat(int host_pid);
  // TODO
};


#endif //GOAT_GOAT_H

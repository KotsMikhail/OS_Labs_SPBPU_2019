#ifndef GOAT_H
#define GOAT_H

#include <conn.h>
#include <bits/types/siginfo_t.h>
#include <semaphore.h>

class Goat
{
public:
  static Goat& GetInstance(int host_pid);
  void Start();
  bool OpenConnection();
private:
  Conn connection;
  sem_t* semaphore;
  int host_pid;

  Goat(int host_pid);
  void Terminate(int signum);
  static int GetRand(int right);
  static void SignalHandler(int signum);
};


#endif //GOAT_GOAT_H

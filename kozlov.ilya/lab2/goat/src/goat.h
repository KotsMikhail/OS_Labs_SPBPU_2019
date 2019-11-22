#ifndef GOAT_H
#define GOAT_H

#include <conn.h>
#include <bits/types/siginfo_t.h>
#include <semaphore.h>
#include <memory.h>

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
  bool need_to_post;

  Goat(int host_pid);
  void Terminate(int signum);
  bool CheckIfSelfMessage(Message& msg);
  static int GetRand(int right);
  static void SignalHandler(int signum);
};


#endif //GOAT_GOAT_H

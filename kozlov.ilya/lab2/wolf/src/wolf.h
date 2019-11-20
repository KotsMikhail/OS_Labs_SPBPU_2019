#ifndef WOLF_H
#define WOLF_H

#include <bits/types/siginfo_t.h>
#include <semaphore.h>
#include <conn.h>
#include <memory.h>
#include "client_info.h"

class Wolf
{
public:
  static Wolf& GetInstance();
  bool OpenConnection();
  void Start();
private:
  Conn connection;
  sem_t* semaphore;
  ClientInfo client_info;
  int current_number;

  Wolf();
  Memory CountStep(Memory& answer);
  void Terminate(int signum);
  static int GetRand();
  static void SignalHandler(int signum, siginfo_t* info, void *ptr);
  // TODO
};


#endif //WOLF_H

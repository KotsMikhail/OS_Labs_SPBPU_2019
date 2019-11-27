#ifndef WOLF_H
#define WOLF_H

#include <csignal>
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
  Wolf(Wolf&) = delete;
  Wolf(const Wolf&) = delete;
  Wolf& operator=(const Wolf&) = delete;
private:
  Conn connection;
  sem_t* semaphore_host;
  sem_t* semaphore_client;
  ClientInfo client_info;
  int current_number;

  Wolf();
  Message CountStep(Message& answer);
  void Terminate(int signum);
  void GetUserNumber();
  static void SignalHandler(int signum, siginfo_t* info, void *ptr);
  // TODO
};


#endif //WOLF_H

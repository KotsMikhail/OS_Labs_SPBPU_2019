#ifndef WOLF_H
#define WOLF_H

#include <bits/types/siginfo_t.h>
#include <semaphore.h>
#include <conn.h>

class Wolf
{
public:
  static int last_err;

  static Wolf GetInstance();
  void Start();
  ~Wolf();
private:
  Conn connection;
  bool client_attached;
  sem_t* semaphore;

  Wolf();
  static void SignalHandler(int signum, siginfo_t* info, void *ptr);
  // TODO
};


#endif //WOLF_H

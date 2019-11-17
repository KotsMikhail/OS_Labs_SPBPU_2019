#ifndef WOLF_H
#define WOLF_H

#include <bits/types/siginfo_t.h>
#include "conn.h"

class Wolf
{
public:
  static Wolf GetInstance();
  void Start();
private:
  Conn connection;

  Wolf();
  static void SignalHandler(int signum, siginfo_t* info, void *ptr);
  // TODO
};


#endif //WOLF_H

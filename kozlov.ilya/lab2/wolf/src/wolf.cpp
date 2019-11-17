#include "wolf.h"

#include <iostream>
#include <cstring>
#include <signal.h>
#include <zconf.h>

Wolf Wolf::GetInstance()
{
  static Wolf inst = Wolf();
  return inst;
}

Wolf::Wolf() : connection(getpid(), true)
{
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = SignalHandler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &act, nullptr);
  sigaction(SIGUSR1, &act, nullptr);
  std::cout << "pid of created wolf is: " << getpid() << std::endl;
}

void Wolf::Start()
{
  // TODO
  while (true)
  {
  }
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr)
{
  std::cout << "Received signal: " << signum << std::endl;
  std::cout << "Signal originates from process: " << ((siginfo_t*)info)->si_pid << std::endl;
}

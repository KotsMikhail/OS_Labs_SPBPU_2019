#include "wolf.h"

#include <iostream>
#include <cstring>
#include <signal.h>
#include <zconf.h>
#include <fcntl.h>

Wolf Wolf::GetInstance()
{
  static Wolf inst = Wolf();
  return inst;
}

Wolf::Wolf() : connection(getpid(), true), client_attached(false)
{
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = SignalHandler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &act, nullptr);
  sigaction(SIGUSR1, &act, nullptr);
  std::cout << "pid of created wolf is: " << getpid() << std::endl;
}

Wolf::~Wolf()
{
  std::cout << "~Wolf()" << std::endl;
  sem_unlink("LAB2");
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Wolf::Start()
{
  // TODO
  // waiting for client
  std::cout << "Waiting for client..." << std::endl;
  pause();
  client_attached = true;
  std::cout << "Client attached!" << std::endl;
  while (true)
  {
    if (!client_attached)
    {
      std::cout << "Waiting for client..." << std::endl;
      pause();
      // TODO
    }
    else
    {
      sleep(5);
      char str[100];
      std::cout << "Going to read..." << std::endl;
      if (connection.Read(str, 100))
      {
        std::cout << "Was read: " << str << std::endl;
        std::string tmp = "Hello from Wolf!";
        connection.Write((void *)tmp.data(), tmp.size());
      }
    }
  }
}
#pragma clang diagnostic pop

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr)
{
  if (signum == SIGUSR1)
  {
    std::cout << "Attaching client with pid = " << info->si_pid << std::endl;
  }
  else if (signum == SIGTERM)
  {
    exit(SIGTERM);
  }
  else
  {
    std::cout << "Unknown signal = " << signum << std::endl;
    exit(signum);
  }
}

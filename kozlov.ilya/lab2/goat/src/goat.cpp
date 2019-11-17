#include "goat.h"

#include <iostream>
#include <zconf.h>
#include <signal.h>

Goat Goat::GetInstance(int host_pid)
{
  static Goat inst = Goat(host_pid);
  return inst;
}

Goat::Goat(int host_pid) : connection(host_pid, false)
{
  kill(host_pid, SIGUSR1);
  std::cout << "host Wolf pid is: " << host_pid << std::endl;
  std::cout << "pid of created Goat is: " << getpid() << std::endl;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Goat::Start()
{
  // TODO
  while (true)
  {
    std::string tmp = "Hello from Goat!";
    if (connection.Write((void *)tmp.data(), tmp.size()))
    {
      sleep(5);
      char str[100];
      connection.Read(str, 100);
      std::cout << "Was read: " << str << std::endl;
    }
  }
}
#pragma clang diagnostic pop
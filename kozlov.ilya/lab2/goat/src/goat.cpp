#include "goat.h"

#include <iostream>
#include <zconf.h>
#include <csignal>

#define SEM_NAME "LAB2"

int Goat::last_err = 0;

Goat Goat::GetInstance(int host_pid)
{
  static Goat inst = Goat(host_pid);
  return inst;
}

Goat::Goat(int host_pid) : connection(host_pid, false)
{
  kill(host_pid, SIGUSR1);
  std::cout << "host Wolf pid is: " << host_pid << std::endl;
  semaphore = sem_open(SEM_NAME, 0);
  if (semaphore == SEM_FAILED)
  {
    std::cout << "ERROR: sem_open failed with error = " << errno << std::endl;
    last_err = errno;
  }
  else
  {
    std::cout << "pid of created Goat is: " << getpid() << std::endl;
  }
}

Goat::~Goat()
{
  sem_close(semaphore);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Goat::Start()
{
  // TODO
  std::string tmp = "Hello from Goat!";
  sem_wait(semaphore);
  connection.Write((void *)tmp.data(), tmp.size());
  sem_post(semaphore);
  while (true)
  {
    sleep(5);
    sem_wait(semaphore);
    char str[100];
    if (connection.Read(str, 100))
    {
      std::cout << "Was read: " << str << std::endl;
      std::string tmp = "Hello from Goat!";
      connection.Write((void *)tmp.data(), tmp.size());
    }
    sem_post(semaphore);
  }
}
#pragma clang diagnostic pop
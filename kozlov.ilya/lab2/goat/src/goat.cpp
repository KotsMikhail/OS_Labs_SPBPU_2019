#include "goat.h"

#include <iostream>
#include <zconf.h>
#include <csignal>
#include <memory.h>
#include <random>

int Goat::last_err = 0;

Goat& Goat::GetInstance(int host_pid)
{
  static Goat inst = Goat(host_pid);
  return inst;
}

Goat::Goat(int host_pid) : connection(host_pid, false)
{
  std::cout << "host Wolf pid is: " << host_pid << std::endl;
  this->host_pid = host_pid;
  signal(SIGTERM, SignalHandler);
  semaphore = sem_open(SEM_NAME, 0);
  if (semaphore == SEM_FAILED)
  {
    std::cout << "ERROR: sem_open failed with error = " << errno << std::endl;
    last_err = errno;
  }
  else
  {
    std::cout << "pid of created Goat is: " << getpid() << std::endl;
    kill(host_pid, SIGUSR1);
  }
}

Goat::~Goat()
{
  std::cout << "~Goat()" << std::endl;
  sem_post(semaphore);
  sem_close(semaphore);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Goat::Start()
{
  struct timespec ts;
  ts.tv_sec = 5;
  ts.tv_nsec = 0;
  while (true)
  {
    sleep(1);
    if (sem_timedwait(semaphore, &ts) == -1)
    {
      kill(host_pid, SIGUSR1);
      exit(errno);
    }
    Memory msg;
    if (connection.Read(&msg, 0))
    {
      std::cout << "Wolf number: " << msg.number << std::endl;
      std::cout << "Status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
      if (msg.status == ALIVE)
      {
        msg.number = GetRand(100);
      }
      else
      {
        msg.number = GetRand(50);
      }
      std::cout << "Goat number: " << msg.number << std::endl;
      connection.Write((void *)&msg, sizeof(msg));
    }
    sem_post(semaphore);
  }
}
#pragma clang diagnostic pop

int Goat::GetRand(int right)
{
  std::random_device rd;
  std::minstd_rand mt(rd());
  std::uniform_int_distribution<int> dist(1, right);
  return dist(mt);
}

void Goat::SignalHandler(int signum)
{
  if (signum == SIGTERM)
  {
    exit(SIGTERM);
  }
}

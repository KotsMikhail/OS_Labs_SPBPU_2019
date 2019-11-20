#include "goat.h"

#include <iostream>
#include <zconf.h>
#include <csignal>
#include <memory.h>
#include <random>
#include <cstring>

Goat& Goat::GetInstance(int host_pid)
{
  static Goat inst = Goat(host_pid);
  return inst;
}

Goat::Goat(int host_pid)
{
  std::cout << "host Wolf pid is: " << host_pid << std::endl;
  this->host_pid = host_pid;
  signal(SIGTERM, SignalHandler);
}

bool Goat::OpenConnection()
{
  bool res = false;
  if (connection.Open(host_pid, false))
  {
    semaphore = sem_open(SEM_NAME, 0);
    if (semaphore == SEM_FAILED)
    {
      std::cout << "ERROR: sem_open failed with error = " << strerror(errno) << std::endl;
    }
    else
    {
      res = true;
      std::cout << "pid of created Goat is: " << getpid() << std::endl;
      kill(host_pid, SIGUSR1);
    }
  }
  return res;
}

void Goat::Terminate(int signum)
{
  kill(host_pid, SIGUSR2);
  std::cout << "Goat::Terminate()" << std::endl;
  if (errno != 0)
  {
    std::cout << "Failing with error = " << strerror(errno) << std::endl;
  }
  if (connection.Close() && sem_post(semaphore) == 0 && sem_close(semaphore) == 0)
  {
    exit(signum);
  }
  std::cout << "Terminating error: " << strerror(errno) << std::endl;
  exit(errno);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Goat::Start()
{
  struct timespec ts;
  ts.tv_sec = 5;
  ts.tv_nsec = 0;
  int skipped_msgs = 0;
  while (true)
  {
    //sleep(1);
    if (sem_timedwait(semaphore, &ts) == -1)
    {
      Terminate(errno);
    }
    Memory msg;
    if (connection.Read(&msg, 0))
    {
      if (msg.owner == GOAT)
      {
        skipped_msgs++;
        std::cout << "Host wrote nothing " << skipped_msgs << " times, skipping..." << std::endl;
        if (skipped_msgs >= MAX_SKIPPED_MSGS)
        {
          Terminate(SIGTERM);
        }
        sem_post(semaphore);
        continue;
      }
      skipped_msgs = 0;
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
      msg.owner = GOAT;
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
  Goat& instance = Goat::GetInstance(30);
  instance.Terminate(signum);
}

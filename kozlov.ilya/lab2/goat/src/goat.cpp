#include "goat.h"

#include <iostream>
#include <zconf.h>
#include <errno.h>
#include <random>
#include <cstring>

Goat& Goat::GetInstance(int host_pid)
{
  static Goat inst(host_pid);
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
    semaphore_host = sem_open(SEM_HOST_NAME, 0);
    semaphore_client = sem_open(SEM_CLIENT_NAME, 0);
    if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED)
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
  semaphore_host = SEM_FAILED;
  semaphore_client = SEM_FAILED;
  if (sem_close(semaphore_client) == -1 || sem_close(semaphore_host) == -1)
  {
    exit(errno);
  }
  if (!connection.Close())
  {
    exit(errno);
  }
  exit(signum);
}

void Goat::Start()
{
  while (true)
  {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIMEOUT;
    if (sem_timedwait(semaphore_client, &ts) == -1)
    {
      if (errno == EINTR)
      {
        continue;
      }
      Terminate(errno);
    }
    Message msg;
    if (connection.Read(&msg))
    {
      std::cout << "---------------- ROUND ----------------" << std::endl;
      std::cout << "Wolf number: " << msg.number << std::endl;
      std::cout << "Status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;
      if (msg.status == Status::ALIVE)
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
    sem_post(semaphore_host);
  }
}

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

#include "goat.h"

#include <iostream>
#include <zconf.h>
#include <errno.h>
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
  need_to_post = false;
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
  if (connection.Close())
  {
    if (need_to_post && sem_post(semaphore) == -1)
    {
      exit(errno);
    }
    if (sem_close(semaphore) == -1)
    {
      exit(errno);
    }
    exit(signum);
  }
  std::cout << "Terminating error: " << strerror(errno) << std::endl;
  exit(errno);
}

void Goat::Start()
{
  while (true)
  {
#ifndef client_fifo
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIMEOUT;
    if (sem_timedwait(semaphore, &ts) == -1)
    {
      Terminate(errno);
    }
    need_to_post = true;
#endif
    Message msg;
    if (connection.Read(&msg))
    {
      if (CheckIfSelfMessage(msg))
      {
        continue;
      }
      std::cout << "---------------- ROUND ----------------" << std::endl;
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
#ifndef client_fifo
    sem_post(semaphore);
    need_to_post = false;
#endif
  }
}

bool Goat::CheckIfSelfMessage(Message& msg)
{
  static int skipped_msgs = 0;
  static struct timespec skip_start;
  bool res = false;
  if (msg.owner == GOAT)
  {
    res = true;
    skipped_msgs++;
    if (skipped_msgs == 1)
    {
      clock_gettime(CLOCK_REALTIME, &skip_start);
    }
    else
    {
      struct timespec cur_time;
      clock_gettime(CLOCK_REALTIME, &cur_time);
      if (cur_time.tv_sec - skip_start.tv_sec >= TIMEOUT)
      {
        Terminate(SIGTERM);
      }
    }
#ifdef client_mq
    connection.Write(&msg, sizeof(msg));
#endif
#ifndef client_fifo
    sem_post(semaphore);
    need_to_post = false;
#endif
  }
  else
  {
    skipped_msgs = 0;
  }
  return res;
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

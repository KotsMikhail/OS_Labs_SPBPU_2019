#include "wolf.h"

#include <iostream>
#include <cstring>
#include <csignal>
#include <zconf.h>
#include <fcntl.h>
#include <random>

Wolf& Wolf::GetInstance()
{
  static Wolf instance;
  return instance;
}

Wolf::Wolf() : client_info(0)
{
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = SignalHandler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &act, nullptr);
  sigaction(SIGUSR1, &act, nullptr);
  sigaction(SIGUSR2, &act, nullptr);
}

bool Wolf::OpenConnection()
{
  bool res = false;
  if (connection.Open(getpid(), true))
  {
    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED)
    {
      std::cout << "ERROR: sem_open failed with error = " << strerror(errno) << std::endl;
    }
    else
    {
      res = true;
      std::cout << "pid of created wolf is: " << getpid() << std::endl;
    }
  }
  return res;
}

void Wolf::Start()
{
  struct timespec ts;
#ifndef host_fifo
  sem_wait(semaphore);
#endif
  std::cout << "Waiting for client..." << std::endl;
  pause();
  std::cout << "Client attached!" << std::endl;
  current_number = GetRand();
  std::cout << "Wolf current number: " << current_number << std::endl;
  Message msg(WOLF, ALIVE, current_number);
  connection.Write(&msg, sizeof(msg));
#ifndef host_fifo
  sem_post(semaphore);
#endif
  while (true)
  {
    if (!client_info.attached)
    {
      std::cout << "Waiting for client..." << std::endl;
#ifndef host_fifo
      sem_wait(semaphore);
#endif
      pause();
      std::cout << "Wolf current number: " << current_number << std::endl;
      msg = Message(WOLF, ALIVE, current_number);
      connection.Write(&msg, sizeof(msg));
#ifndef host_fifo
      sem_post(semaphore);
#endif
    }
    else
    {
#ifndef host_fifo
      clock_gettime(CLOCK_REALTIME, &ts);
      ts.tv_sec += TIMEOUT;
      if (sem_timedwait(semaphore, &ts) == -1)
      {
        kill(client_info.pid, SIGTERM);
        client_info = ClientInfo(0);
        continue;
      }
#endif
      if (connection.Read(&msg))
      {
        if (CheckIfSelfMessage(msg))
        {
          continue;
        }
        std::cout << "---------------- ROUND ----------------" << std::endl;
        std::cout << "Wolf current number: " << current_number << std::endl;
        std::cout << "Goat current number: " << msg.number << std::endl;
        std::cout << "Goat current status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
        msg = CountStep(msg);
        if (client_info.attached)
        {
          std::cout << "Wolf new number: " << current_number << std::endl;
          std::cout << "Goat new status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
          connection.Write(&msg, sizeof(msg));
        }
      }
#ifndef host_fifo
      sem_post(semaphore);
#endif
    }
  }
}

bool Wolf::CheckIfSelfMessage(Message& msg)
{
  static struct timespec skip_start;
  bool res = false;
  if (msg.owner == WOLF)
  {
    res = true;
    client_info.skipped_msgs++;
    if (client_info.skipped_msgs == 1)
    {
      clock_gettime(CLOCK_REALTIME, &skip_start);
    }
    else
    {
      struct timespec cur_time;
      clock_gettime(CLOCK_REALTIME, &cur_time);
      if (cur_time.tv_sec - skip_start.tv_sec >= TIMEOUT)
      {
        kill(client_info.pid, SIGTERM);
        client_info = ClientInfo(0);
      }
    }
#ifdef host_mq
    connection.Write(&msg, sizeof(msg));
#endif
#ifndef host_fifo
    sem_post(semaphore);
#endif
  }
  else
  {
    client_info.skipped_msgs = 0;
  }
  return res;
}

Message Wolf::CountStep(Message& answer)
{
  Message msg;
  if ((answer.status == ALIVE && abs(current_number - answer.number) <= 70) ||
      (answer.status == DEAD && abs(current_number - answer.number) <= 20))
  {
    client_info.deads_num = 0;
  }
  else
  {
    msg.status = DEAD;
    client_info.deads_num++;
    if (client_info.deads_num == 2)
    {
      kill(client_info.pid, SIGTERM);
      client_info = ClientInfo(0);
      // This is new data for new clients
      msg.status = ALIVE;
    }
  }
  current_number = GetRand();
  msg.number = current_number;
  return msg;
}

int Wolf::GetRand()
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(1, 100);
  return dist(mt);
}

void Wolf::Terminate(int signum)
{
  std::cout << "Wolf::Terminate()" << std::endl;
  if (connection.Close() && sem_unlink(SEM_NAME) == 0)
  {
    exit(signum);
  }
  exit(errno);
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr)
{
  static Wolf& instance = GetInstance();
  switch (signum)
  {
    case SIGUSR1:
    {
      if (instance.client_info.attached)
      {
        std::cout << "This host can handle only one client!" << std::endl;
      }
      else
      {
        std::cout << "Attaching client with pid = " << info->si_pid << std::endl;
        instance.client_info = ClientInfo(info->si_pid);
      }
      break;
    }
    case SIGUSR2: // client failed
    {
      std::cout << "Client failed" << std::endl;
      if (instance.client_info.pid == info->si_pid)
      {
        instance.client_info = ClientInfo(0);
      }
      break;
    }
    default:
    {
      if (instance.client_info.attached)
      {
        kill(instance.client_info.pid, SIGTERM);
        instance.client_info = ClientInfo(0);
      }
      instance.Terminate(signum);
    }
  }
}

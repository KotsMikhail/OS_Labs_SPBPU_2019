#include "wolf.h"

#include <iostream>
#include <cstring>
#include <csignal>
#include <zconf.h>
#include <fcntl.h>
#include <random>

//ClientInfo Wolf::client_info(0);

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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Wolf::Start()
{
  struct timespec ts;
  ts.tv_sec = 5;
  ts.tv_nsec = 0;
  std::cout << "Waiting for client..." << std::endl;
  pause();
  while (!client_info.attached)
  {
    sleep(1);
  }
  std::cout << "Client attached!" << std::endl;
  sem_wait(semaphore);
  current_number = GetRand();
  Memory msg(WOLF, ALIVE, current_number);
  connection.Write(&msg, sizeof(msg));
  sem_post(semaphore);
  while (true)
  {
    // sleep(1);
    if (!client_info.attached)
    {
      std::cout << "Waiting for client..." << std::endl;
      pause();
    }
    else
    {
      if (sem_timedwait(semaphore, &ts) == -1)
      {
        kill(client_info.pid, SIGTERM);
        client_info = ClientInfo(0);
        continue;
      }
      if (connection.Read(&msg, 0))
      {
        if (msg.owner == WOLF)
        {
          client_info.skipped_msgs++;
          std::cout << "Client wrote nothing " << client_info.skipped_msgs << "times, skipping..." << std::endl;
          if (client_info.skipped_msgs >= MAX_SKIPPED_MSGS)
          {
            kill(client_info.pid, SIGTERM);
            client_info = ClientInfo(0);
          }
          sem_post(semaphore);
          continue;
        }
        client_info.skipped_msgs = 0;
        std::cout << "Wolf current number: " << current_number << std::endl;
        std::cout << "Goat current number: " << msg.number << std::endl;
        std::cout << "Goat current status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
        msg = CountStep(msg);
        std::cout << "Wolf new number: " << current_number << std::endl;
        std::cout << "Goat new status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
        connection.Write(&msg, sizeof(msg));
      }
      sem_post(semaphore);
    }
  }
}
#pragma clang diagnostic pop

Memory Wolf::CountStep(Memory& answer)
{
  Memory msg;
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
  static Wolf& instance = Wolf::GetInstance();
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

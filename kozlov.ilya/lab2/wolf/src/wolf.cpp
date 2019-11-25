#include "wolf.h"

#include <iostream>
#include <cstring>
#include <zconf.h>
#include <fcntl.h>
#include <random>
#include <errno.h>
#include <const.h>

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
  sigaction(SIGINT, &act, nullptr);
}

bool Wolf::OpenConnection()
{
  bool res = false;
  if (connection.Open(getpid(), true))
  {
    semaphore_host = sem_open(Const::SEM_HOST_NAME, O_CREAT, 0666, 0);
    semaphore_client = sem_open(Const::SEM_CLIENT_NAME, O_CREAT, 0666, 0);
    if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED)
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
  Message msg;
  std::cout << "Waiting for client..." << std::endl;
  pause();
  std::cout << "Client attached!" << std::endl;
  sem_post(semaphore_client);
  while (true)
  {
    if (!client_info.attached)
    {
      std::cout << "Waiting for client..." << std::endl;
      sem_wait(semaphore_client);
      while (!client_info.attached)
      {
        pause();
      }
      sem_post(semaphore_client);
    }
    else
    {
      clock_gettime(CLOCK_REALTIME, &ts);
      ts.tv_sec += Const::TIMEOUT;
      if (sem_timedwait(semaphore_host, &ts) == -1)
      {
        if (errno == EINTR)
        {
          continue;
        }
        kill(client_info.pid, SIGTERM);
        client_info = ClientInfo(0);
        continue;
      }
      if (connection.Read(&msg))
      {
        std::cout << "---------------- ROUND ----------------" << std::endl;
        std::cout << "Goat current number: " << msg.number << std::endl;
        std::cout << "Goat current status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;
        GetUserNumber();
        msg = CountStep(msg);
        if (client_info.attached)
        {
          std::cout << "Goat new status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;
          connection.Write(&msg, sizeof(msg));
        }
      }
      sem_post(semaphore_client);
    }
  }
}

Message Wolf::CountStep(Message& answer)
{
  Message msg(Status::ALIVE, current_number);
  if ((answer.status == Status::ALIVE && abs(current_number - answer.number) <= 70) ||
      (answer.status == Status::DEAD && abs(current_number - answer.number) <= 20))
  {
    client_info.deads_num = 0;
  }
  else
  {
    msg.status = Status::DEAD;
    client_info.deads_num++;
    if (client_info.deads_num == 2)
    {
      kill(client_info.pid, SIGTERM);
      client_info = ClientInfo(0);
    }
  }
  return msg;
}

void Wolf::GetUserNumber()
{
  std::cout << "Enter wolf new number: ";
  bool is_set = false;
  std::string input;
  while (!is_set)
  {
    std::getline(std::cin, input);
    if (input.find_last_not_of("0123456789") != std::string::npos)
    {
      std::cout << "Wrong input format, should be only one integer!" << std::endl;
    }
    else
    {
      try
      {
        current_number = std::stoi(input);
        if (current_number < 1 || current_number > 100)
        {
          std::cout << "Wrong number, should be 1 <= num <= 100!" << std::endl;
        }
        else
        {
          is_set = true;
        }
      }
      catch (const std::invalid_argument& exp)
      {
        std::cout << "Can't get integer from input!" << std::endl;
      }
    }
    if (!is_set)
    {
      std::cout << "Try again: ";
    }
  }
}

void Wolf::Terminate(int signum)
{
  std::cout << "Wolf::Terminate()" << std::endl;
  semaphore_client = SEM_FAILED;
  semaphore_host = SEM_FAILED;
  if (sem_unlink(Const::SEM_HOST_NAME) == -1 || sem_unlink(Const::SEM_CLIENT_NAME) == -1)
  {
    exit(errno);
  }
  if (!connection.Close())
  {
    exit(errno);
  }
  exit(signum);
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
    case SIGUSR2:
    {
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
        kill(instance.client_info.pid, signum);
        instance.client_info = ClientInfo(0);
      }
      instance.Terminate(signum);
    }
  }
}

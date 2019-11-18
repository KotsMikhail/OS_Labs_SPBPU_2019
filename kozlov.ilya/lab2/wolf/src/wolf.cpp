#include "wolf.h"

#include <iostream>
#include <cstring>
#include <signal.h>
#include <zconf.h>
#include <fcntl.h>

#define SEM_NAME "LAB2"

int Wolf::last_err = 0;

Wolf& Wolf::GetInstance()
{
  static Wolf inst = Wolf();
  return inst;
}

Wolf::Wolf() : connection(getpid(), true), client_info(0)
{
  last_err = 0;
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_sigaction = SignalHandler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &act, nullptr);
  sigaction(SIGUSR1, &act, nullptr);
  semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
  if (semaphore == SEM_FAILED)
  {
    std::cout << "ERROR: sem_open failed with error = " << errno << std::endl;
    last_err = errno;
  }
  else
  {
    std::cout << "pid of created wolf is: " << getpid() << std::endl;
  }
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
  std::cout << "Waiting for client..." << std::endl;
  pause();
  std::cout << "Client attached!" << std::endl;
  sem_wait(semaphore);
  current_number = rand() % 100 + 1;
  Memory msg(ALIVE, current_number);
  connection.Write(&msg, sizeof(msg));
  sem_post(semaphore);
  while (true)
  {
    sleep(5);
    std::cout << "Client attached = " << client_info.attached << " with pid = " << client_info.pid << std::endl;
    if (!client_info.attached)
    {
      std::cout << "Waiting for client..." << std::endl;
      pause();
    }
    else
    {
      sem_wait(semaphore);
      if (connection.Read(&msg, 0))
      {
        std::cout << "Number: " << msg.number << std::endl;
        std::cout << "Status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
        msg = CountStep(msg);
        if (client_info.attached)
        {
          connection.Write(&msg, sizeof(msg));
        }
      }
      sem_post(semaphore);
    }
  }
}
#pragma clang diagnostic pop

Memory Wolf::CountStep(Memory& answer)
{
  Memory msg;
  if (answer.status == ALIVE && abs(current_number - answer.number) <= 70)
  {
    msg.status = ALIVE;
  }
  else if (answer.status == DEAD && abs(current_number - answer.number) <= 20)
  {
    msg.status = ALIVE;
  }
  else
  {
    msg.status = DEAD;
    client_info.deads_num++;
    if (client_info.deads_num == 2)
    {
      kill(client_info.pid, SIGTERM);
      client_info = ClientInfo(0);
    }
  }
  current_number = rand() % 100 + 1;
  msg.number = current_number;
  return msg;
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr)
{
  if (signum == SIGUSR1)
  {
    std::cout << "Attaching client with pid = " << info->si_pid << std::endl;
    Wolf::GetInstance().client_info = ClientInfo(info->si_pid);
    std::cout << "Client attached = " << Wolf::GetInstance().client_info.attached
      << " with pid = " << Wolf::GetInstance().client_info.pid << std::endl;
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

#include "conn.h"

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/user.h>
#include <fcntl.h>
#include <cstring>

int shmid;
sem_t* semaphore;

struct memory
{
  char message[100];
  int size;
};

Conn::Conn(size_t id, bool create)
{
  int shmflg = 0666;
  int semflg = O_CREAT;
  if (create)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
    shmflg |= IPC_CREAT;
    semaphore = sem_open("LAB2", semflg, 0666, 1);
    if (semaphore == SEM_FAILED)
    {
      std::cout << "ERROR: sem_open failed with error = " << errno << std::endl;
    }
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
    semaphore = sem_open("LAB2", semflg);
    if (semaphore == SEM_FAILED)
    {
      std::cout << "ERROR: sem_open failed with error = " << errno << std::endl;
    }
  }
  key_t key = id;
  int size = 100;
  if ((shmid = shmget(key, sizeof(memory), shmflg)) == -1)
  {
    std::cout << "ERROR: shmget failed" << std::endl;
  }
  else
  {
    std::cout << "shmget returned id = " << shmid << std::endl;
  }
}

bool Conn::Read(void* buf, size_t count)
{
  std::cout << "Conn::Read()" << std::endl;
  memory* shm_buf;
  //sem_wait(semaphore);
  bool success = true;
  shm_buf = (memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    std::cout << "shm_buf->message = " << shm_buf->message << std::endl;
    //*((std::string *)buf) = shm_buf->message;
    //char tmp[100];
    strncpy((char*)buf, shm_buf->message, shm_buf->size);
    //*(char**)buf = tmp;
    shmdt(shm_buf);
  }
  //sem_post(semaphore);
  std::cout << "exit Conn::Read()" << std::endl;
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  std::cout << "Conn::Write()" << std::endl;
  memory* shm_buf;
  //sem_wait(semaphore);
  bool success = true;
  shm_buf = (memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    //shm_buf->message = *((std::string *)buf);
    strncpy(shm_buf->message, (char *)buf, count);
    shm_buf->size = count;
    shmdt(shm_buf);
  }
  //sem_post(semaphore);
  std::cout << "exit Conn::Write()" << std::endl;
  return success;
}

Conn::~Conn()
{
  shmctl(shmid, IPC_RMID, nullptr);
  sem_close(semaphore);
}

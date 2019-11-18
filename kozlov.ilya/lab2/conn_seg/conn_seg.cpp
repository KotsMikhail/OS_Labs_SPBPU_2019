#include "conn.h"

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>

int shmid;

struct memory
{
  char message[100];
  int size;
};

Conn::Conn(size_t id, bool create)
{
  int shmflg = 0666;
  if (create)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
    shmflg |= IPC_CREAT;
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
  }
  if ((shmid = shmget(id, sizeof(memory), shmflg)) == -1)
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
  bool success = true;
  shm_buf = (memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    if (shm_buf->size == 0)
    {
      success = false;
    }
    else
    {
      std::cout << "shm_buf->message = " << shm_buf->message << std::endl;
      strncpy((char*) buf, shm_buf->message, shm_buf->size);
      ((char*) buf)[shm_buf->size] = '\0';
      shmdt(shm_buf);
    }
  }
  std::cout << "exit Conn::Read()" << std::endl;
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  std::cout << "Conn::Write()" << std::endl;
  memory* shm_buf;
  bool success = true;
  shm_buf = (memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    strncpy(shm_buf->message, (char *)buf, count);
    shm_buf->size = count;
    shmdt(shm_buf);
  }
  std::cout << "exit Conn::Write()" << std::endl;
  return success;
}

Conn::~Conn()
{
  std::cout << "Conn::~Conn()" << std::endl;
  shmctl(shmid, IPC_RMID, nullptr);
}

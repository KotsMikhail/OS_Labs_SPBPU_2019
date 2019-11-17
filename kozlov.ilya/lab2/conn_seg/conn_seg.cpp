#include "conn.h"

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/user.h>

int shmid;

Conn::Conn(size_t id, bool create)
{
  int shmflg = 0666;
  if (create)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
    shmflg |= IPC_CREAT | IPC_EXCL;
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
  }
  key_t key = id;
  int size = PAGE_SIZE;
  if ((shmid = shmget(key, size, shmflg)) == -1)
  {
    std::cout << "ERROR: shmget: shmget failed" << std::endl;
  }
  else
  {
    std::cout << "shmget returned id = " << shmid << std::endl;
  }
}

bool Conn::Read(void* buf, size_t count)
{
  void* shm_buf;
  bool success = true;
  if ((shm_buf = (char *)shmat(shmid, nullptr, 0)) == (char *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    buf = shm_buf;
    shmdt(shm_buf);
  }
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  void* shm_buf;
  bool success = true;
  if ((shm_buf = shmat(shmid, nullptr, 0)) == (char *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    shm_buf = buf;
    shmdt(shm_buf);
  }
  return success;
}

Conn::~Conn()
{
  shmctl(shmid, IPC_RMID, nullptr);
}

#include <conn.h>
#include <memory.h>

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>

int shmid;

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
  if ((shmid = shmget(id, sizeof(Memory), shmflg)) == -1)
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
  Memory* shm_buf;
  bool success = true;
  shm_buf = (Memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (Memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    ((Memory *)buf)->status = shm_buf->status;
    ((Memory *)buf)->number = shm_buf->number;
    shmdt(shm_buf);
  }
  std::cout << "exit Conn::Read()" << std::endl;
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  std::cout << "Conn::Write()" << std::endl;
  Memory* shm_buf;
  bool success = true;
  shm_buf = (Memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (Memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    shm_buf->status = ((Memory *)buf)->status;
    shm_buf->number = ((Memory *)buf)->number;
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

#include <conn.h>
#include <memory.h>

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>

int shmid;
bool owner;

bool Conn::Open(size_t id, bool create)
{
  bool res = false;
  owner = create;
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
    res = true;
    std::cout << "shmget returned id = " << shmid << std::endl;
  }
  return res;
}

bool Conn::Read(void* buf, size_t count)
{
  Memory* shm_buf;
  bool success = true;
  shm_buf = (Memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (Memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    *((Memory *)buf) = *shm_buf;
    shmdt(shm_buf);
  }
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  Memory* shm_buf;
  bool success = true;
  shm_buf = (Memory *)shmat(shmid, nullptr, 0);
  if (shm_buf == (Memory *)-1) {
    std::cout << "ERROR: shmat can't attach to memory";
    success = false;
  }
  else
  {
    *shm_buf = *((Memory *)buf);
    shmdt(shm_buf);
  }
  return success;
}

bool Conn::Close()
{
  bool res = true;
  if (owner && shmctl(shmid, IPC_RMID, nullptr) < 0)
  {
    res = false;
  }
  return res;
}

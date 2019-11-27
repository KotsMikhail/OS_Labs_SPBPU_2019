#include <conn.h>
#include <memory.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <error.h>

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
  if ((this->id = shmget(id, sizeof(Message), shmflg)) == -1)
  {
    std::cout << "ERROR: shmget failed, error = " << strerror(errno) << std::endl;
  }
  else
  {
    res = true;
    std::cout << "shmget returned id = " << this->id << std::endl;
  }
  return res;
}

bool Conn::Read(void* buf, size_t count)
{
  Message* shm_buf;
  bool success = true;
  shm_buf = (Message *)shmat(id, nullptr, 0);
  if (shm_buf == (Message *)-1) {
    std::cout << "ERROR: shmat can't attach to memory" << std::endl;
    success = false;
  }
  else
  {
    *((Message *)buf) = *shm_buf;
    shmdt(shm_buf);
  }
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  Message* shm_buf;
  bool success = true;
  shm_buf = (Message *)shmat(id, nullptr, 0);
  if (shm_buf == (Message *)-1)
  {
    std::cout << "ERROR: shmat can't attach to memory" << std::endl;
    success = false;
  }
  else
  {
    *shm_buf = *((Message *)buf);
    shmdt(shm_buf);
  }
  return success;
}

bool Conn::Close()
{
  bool res = true;
  if (owner && shmctl(id, IPC_RMID, nullptr) < 0)
  {
    res = false;
  }
  return res;
}

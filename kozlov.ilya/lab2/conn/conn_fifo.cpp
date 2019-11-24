#include <conn.h>
#include <memory.h>

#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <zconf.h>
#include <error.h>

bool Conn::Open(size_t id, bool create)
{
  bool res = false;
  owner = create;
  name = "/tmp/lab2_pipe";
  int fifoflg = 0777;
  if (create)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
  }
  if (owner && mkfifo(name.c_str(), fifoflg) == -1)
  {
    std::cout << "ERROR: mkfifo failed, error = " << strerror(errno) << std::endl;
  }
  else
  {
    this->id = open(name.c_str(), O_RDWR);
    if (this->id == -1)
    {
      std::cout << "ERROR: open failed, error = " << strerror(errno) << std::endl;
    }
    else
    {
      res = true;
    }
  }
  return res;
}

bool Conn::Read(void* buf, size_t count)
{
  Message shm_buf;
  bool success = false;
  if (read(id, &shm_buf, count) == -1)
  {
    std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
  }
  else
  {
    *((Message*) buf) = shm_buf;
    success = true;
  }
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  bool success = false;
  if (write(id, buf, count) == -1)
  {
    std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
  }
  else
  {
    success = true;
  }
  return success;
}

bool Conn::Close()
{
  bool res = true;
  if (close(id) < 0 || (owner && remove(name.c_str()) < 0))
  {
    res = false;
  }
  return res;
}

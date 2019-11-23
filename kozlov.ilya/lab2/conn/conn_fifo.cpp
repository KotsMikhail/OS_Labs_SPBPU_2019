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

#define FIFO_NAME "/tmp/lab2_pipe"

int fd;
bool owner;

bool Conn::Open(size_t id, bool create)
{
  bool res = false;
  owner = create;
  int fifoflg = 0777;
  if (create)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
  }
  if (owner && mkfifo(FIFO_NAME, fifoflg) == -1)
  {
    std::cout << "ERROR: mkfifo failed, error = " << strerror(errno) << std::endl;
  }
  else
  {
    res = true;
  }
  return res;
}

bool Conn::Read(void* buf, size_t count)
{
  Message shm_buf;
  bool success = false;
  if ((fd = open(FIFO_NAME, O_RDONLY)) == -1)
  {
    std::cout << "ERROR: can't open pipe for reading, error = " << strerror(errno) << std::endl;
  }
  else
  {
    if (read(fd, &shm_buf, count) == -1)
    {
      std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
    }
    else
    {
      *((Message*) buf) = shm_buf;
      success = true;
    }
    close(fd);
  }
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  bool success = false;
  if ((fd = open(FIFO_NAME, O_WRONLY)) == -1)
  {
    std::cout << "ERROR: can't open pipe for writing, error = " << strerror(errno) << std::endl;
  }
  else
  {
    if (write(fd, buf, count) == -1)
    {
      std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
    }
    else
    {
      success = true;
    }
    close(fd);
  }
  return success;
}

bool Conn::Close()
{
  bool res = true;
  if (owner && remove(FIFO_NAME) < 0)
  {
    res = false;
  }
  return res;
}

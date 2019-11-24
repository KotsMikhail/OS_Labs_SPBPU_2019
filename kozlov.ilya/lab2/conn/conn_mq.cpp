#include <conn.h>
#include <memory.h>
#include <iostream>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <error.h>

bool Conn::Open(size_t id, bool create)
{
  bool res = false;
  owner = create;
  name = "/LAB2_QUEUE";
  int mqflg = O_RDWR;
  int mqperm = 0666;
  if (owner)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
    mqflg |= O_CREAT;
    int maxmsg = 1;
    int msgsize = sizeof(Message);
    struct mq_attr attr = ((struct mq_attr){0, maxmsg, msgsize, 0, {0}});
    this->id = mq_open(name.c_str(), mqflg, mqperm, &attr);
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
    this->id = mq_open(name.c_str(), mqflg);
  }
  if (this->id == -1)
  {
    std::cout << "ERROR: mq_open failed, errno = " << strerror(errno) << std::endl;
  }
  else
  {
    std::cout << "mq_open returned id = " << this->id << std::endl;
    res = true;
  }
  return res;
}

bool Conn::Read(void* buf, size_t count)
{
  Message mq_buf;
  bool success = true;
  if (mq_receive(id, (char *)&mq_buf, count, nullptr) == -1)
  {
    std::cout << "ERROR: mq_recieve failed, errno = " << strerror(errno) << std::endl;
    success = false;
  }
  else
  {
    *((Message*) buf) = mq_buf;
  }
  return success;
}

bool Conn::Write(void* buf, size_t count)
{
  bool success = false;
  if (mq_send(id, (char*)buf, count, 0) == -1)
  {
    std::cout << "ERROR: mq_send failed, errno = " << strerror(errno) << std::endl;
  }
  else
  {
    success = true;
  }
  return success;
}

bool Conn::Close()
{
  bool res = false;
  if (mq_close(id) == 0)
  {
    if (!owner)
    {
      res = true;
    }
    else if (owner && mq_unlink(name.c_str()) == 0)
    {
      res = true;
    }
  }
  return res;
}

#include <conn.h>
#include <memory.h>

#include <iostream>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>

#define QUEUE_NAME "/LAB2_QUEUE"
#define QUEUE_MAXMSG  1 /* Maximum number of messages. */
#define QUEUE_MSGSIZE sizeof(Message) /* Length of message. */
#define QUEUE_ATTR_INITIALIZER ((struct mq_attr){0, QUEUE_MAXMSG, QUEUE_MSGSIZE, 0, {0}})

mqd_t mqid;
bool owner;

bool Conn::Open(size_t id, bool create)
{
  bool res = false;
  owner = create;
  int mqflg = O_RDWR;
  int mqperm = 0666;
  if (owner)
  {
    std::cout << "Creating connection with id = " << id << std::endl;
    mqflg |= O_CREAT;
    struct mq_attr attr = QUEUE_ATTR_INITIALIZER;
    mqid = mq_open(QUEUE_NAME, mqflg, mqperm, &attr);
  }
  else
  {
    std::cout << "Getting connection with id = " << id << std::endl;
    mqid = mq_open(QUEUE_NAME, mqflg);
  }
  if (mqid == -1)
  {
    std::cout << "ERROR: mq_open failed, errno = " << strerror(errno) << std::endl;
  }
  else
  {
    std::cout << "mq_open returned id = " << mqid << std::endl;
    res = true;
  }
  return res;
}

bool Conn::Read(void* buf, size_t count)
{
  Message mq_buf;
  bool success = true;
  if (mq_receive(mqid, (char *)&mq_buf, QUEUE_MSGSIZE, nullptr) == -1)
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
  if (count <= QUEUE_MSGSIZE)
  {
    if (mq_send(mqid, (char*)buf, count, 0) == -1)
    {
      std::cout << "ERROR: mq_send failed, errno = " << strerror(errno) << std::endl;
    }
    else
    {
      success = true;
    }
  }
  return success;
}

bool Conn::Close()
{
  bool res = false;
  if (mq_close(mqid) == 0)
  {
    if (!owner)
    {
      res = true;
    }
    else if (owner && mq_unlink(QUEUE_NAME) == 0)
    {
      res = true;
    }
  }
  return res;
}

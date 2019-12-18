//
// Created by peter on 11/22/19.
//

#include <fcntl.h>
#include <mqueue.h>
#include <iostream>
#include <cstring>
#include <sstream>

#include "../interface/conn.h"

void Connection::open( const std::string &id, bool is_create )
{
  m_is_owner = is_create;
  m_id = "/" + id;

  if (m_is_owner)
    mq_unlink(m_id.c_str());

  unsigned flags = O_RDWR;
  const int perm = 0666;

  if (is_create)
  {
    //std::cout << "Creating message queue with name = " << id << std::endl;
    flags |= O_CREAT;

    struct mq_attr attr = ((struct mq_attr){0, 1, sizeof(int), 0, {0}});
    m_desc[0] = mq_open(m_id.c_str(), flags, perm, &attr);
  }
  else
  {
    //std::cout << "Connecting to message queue with name = " << id << std::endl;
    m_desc[0] = mq_open(m_id.c_str(), flags);
  }

  if (m_desc[0] == -1)
    std::runtime_error("Failed to open message queue");
  //std::cout << "Message queue descriptor = " << m_desc[0] << std::endl;
} // end of 'Connection::open' function

int Connection::read()
{
  int message;

  if (mq_receive(m_desc[0], (char *)&message, sizeof(int), nullptr) == -1)
    std::runtime_error("Failed to receive message");

  return message;
} // end of 'Connection::read' function

void Connection::write( int message )
{
  if (mq_send(m_desc[0], (char*)&message, sizeof(int), 0) == -1)
    std::runtime_error("Failed to send message");
} // end of 'Connection::write' function

void Connection::close()
{
  if (mq_close(m_desc[0]) != 0)
    std::cout << "ERROR: Failed to close mq descriptor = " << m_desc[0];
  else
    if (m_is_owner && mq_unlink(m_id.c_str()) != 0)
      std::cout << "ERROR: Failed to unlink message queue with name = " << m_id;
} // end of 'Connection::close' function

//
// Created by peter on 11/22/19.
//

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>

#include "../interface/conn.h"

void Connection::open( const std::string &id, bool is_create )
{
  m_is_owner = is_create;
  m_id = id;

  // unlink old socket just in case
  if (m_is_owner)
    unlink(id.c_str());

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, id.c_str(), sizeof(addr.sun_path) - 1);

  if (is_create)
  {
    m_desc[0] = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_desc[0] == -1)
      throwError("Failed to create socket");

    if (bind(m_desc[0], (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
      throwError("Failed to bind address to socket");

    if (listen(m_desc[0], 1) == -1)
      throwError("Failed to listen for connections on socket");

    m_desc[1] = accept(m_desc[0], nullptr, nullptr);
    if (m_desc[1] == -1)
      throwError("Failed to accept connection on socket");
  }
  else
  {
    m_desc[1] = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_desc[1] == -1)
      throwError("Failed to get socket");

    if (connect(m_desc[1], (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
      throwError("Failed to connect to socket");
  }
} // end of 'Connection::open' function

void Connection::write( int message )
{
  if (send(m_desc[1], &message, sizeof(int), MSG_NOSIGNAL) == -1)
    throwError("Failed to send message");
} // end of 'Connection::write' function

int Connection::read()
{
  int message;

  if (recv(m_desc[1], &message, sizeof(int), 0) == -1)
    throwError("Failed to receive message");

  return message;
} // end of 'Connection::read' function

void Connection::close()
{
  if (::close(m_desc[1]) == -1)
    std::cout << "ERROR: Failed to close connected socket handle, errno = " << strerror(errno) << std::endl;

  if (m_is_owner)
  {
    if (::close(m_desc[0]) == -1)
      std::cout << "ERROR: Failed to close original socket handle, errno = " << strerror(errno) << std::endl;

    if (unlink(m_id.c_str()) == -1)
      std::cout << "ERROR: Failed to unlink socket, errno = " << strerror(errno) << std::endl;
  }
} // end of 'Connection::close' function

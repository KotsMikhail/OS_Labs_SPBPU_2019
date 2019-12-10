//
// Created by peter on 11/22/19.
//

#include <syslog.h>
#include <csignal>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>

#include "../interface/conn.h"

void Connection::open( const std::string &id, bool is_create )
{
  m_is_owner = is_create;
  m_id = id;

  // unlink old fifo file just in case
  if (m_is_owner)
    unlink(id.c_str());

  int flags = S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH;
  if (is_create && mkfifo(id.c_str(), flags) == -1)
    throwError("Failed to create fifo file");

  m_desc[0] = ::open(id.c_str(), O_RDWR);
  if (m_desc[0] == -1)
    throwError("Failed to open fifo file");

  std::cout << "Opened fifo connection" << std::endl;
} // end of 'Connection::open' function

void Connection::write( int message )
{
  if (::write(m_desc[0], &message, sizeof(int)) == -1)
    throwError("Failed to write message");
} // end of 'Connection::write' function

int Connection::read()
{
  int message;

  if (::read(m_desc[0], &message, sizeof(int)) == -1)
    throwError("Failed to read message");

  return message;
} // end of 'Connection::read' function

void Connection::close()
{
  if (::close(m_desc[0]) == -1)
    std::cout << "ERROR: Failed to close file handle, errno = " << strerror(errno) << std::endl;

  if (m_is_owner)
    if (unlink(m_id.c_str()) == -1)
      std::cout << "ERROR: Failed to unlink file, errno = " << strerror(errno) << std::endl;
} // end of 'Connection::close' function

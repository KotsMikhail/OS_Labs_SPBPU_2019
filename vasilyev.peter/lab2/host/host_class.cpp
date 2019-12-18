//
// Created by peter on 11/22/19.
//

#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <fcntl.h>
#include <cstring>
#include <ctime>

#include "host_class.h"
#include "../client/client.h"
#include "../interface/conn.h"

const std::string Host::CONNECTION_NAME = "Lab2Connection";
const std::string Host::HOST_SEMAPHORE_NAME = "/HostSemaphore";
const std::string Host::CLIENT_SEMAPHORE_NAME = "/ClientSemaphore";
const int Host::EXIT_MSG = -1;

Host::Host()
{
  // unlink old semaphores just in case
  sem_unlink(HOST_SEMAPHORE_NAME.c_str());
  sem_unlink(CLIENT_SEMAPHORE_NAME.c_str());

  std::cout << "Creating host semaphore" << std::endl;
  m_host_semaphore = sem_open(HOST_SEMAPHORE_NAME.c_str(), O_CREAT, 0666, 0);
  m_host_semaphore = sem_open(HOST_SEMAPHORE_NAME.c_str(), O_CREAT, 0666, 0);
  if (m_host_semaphore == SEM_FAILED)
  {
    std::ostringstream ss;
    ss << "Failed to open host semaphore, errno = " << strerror(errno);
    throw std::runtime_error(ss.str());
  }

  std::cout << "Creating client semaphore" << std::endl;
  m_client_semaphore = sem_open(CLIENT_SEMAPHORE_NAME.c_str(), O_CREAT, 0666, 0);
  if (m_client_semaphore == SEM_FAILED)
  {
    std::ostringstream ss;
    ss << "Failed to open client semaphore, errno = " << strerror(errno);
    throw std::runtime_error(ss.str());
  }
} // end of 'Host::Host' function

Host &Host::getInstance()
{
  static Host instance;

  return instance;
} // end of 'Host::getInstance' function

void Host::start()
{
  std::cout << "Host start" << std::endl;

  // client is already blocked
  // unblock host
  sem_post(m_host_semaphore);

  // create client child process
  switch (fork())
  {       
    case -1:
      // fork failed
      throw std::runtime_error("Failed to create client process");
    case 0:
      // Inside child process
      // sleep to ensure host create connection 
      sleep(1);
      // Create and start client
      Client(CONNECTION_NAME, m_host_semaphore, m_client_semaphore).start();
      break;
    default:
      // Inside parent process
      std::cout << "Successfully created client process" << std::endl;
      m_connection.open(CONNECTION_NAME, true);
      doWork();
      close();
      break;
  }
} // end of 'Host::start' function

void Host::doWork()
{
  while (true)
  {
    // print command menu
    printMenu();
    
    // read command
    switch (readCommand())
    {
      case Command::COMMAND_COUNT:
      case Command::UNKNOWN:
        std::cout << "Unknown command!" << std::endl;
        break;
      case Command::EXIT:
        std::cout << "Sending 'exit' message to client..." << std::endl;
        // send exit message
        m_connection.write(EXIT_MSG);
        blockAndWait();
        std::cout << "Exiting..." << std::endl;
        return;
      case Command::INPUT_DATE:
        // read date
        int date = readDate();
        std::cout << "Date: " << date << std::endl;

        // send date
        m_connection.write(date);
        blockAndWait();

        // read response
        int temperature = m_connection.read();
        std::cout << "Temperature is " << temperature << " degrees." << std::endl;
        break;
    }
  }
} // end of 'Host::doWork' function

Host::Command Host::readCommand()
{
  std::string command;
  std::cin >> command;

  try
  {
    int num = std::stoi(command) - 1;
    if (num < 0 || num >= static_cast<int>(Command::COMMAND_COUNT))
      throw std::exception();
    
    return static_cast<Command>(num);
  }
  catch (std::exception &e)
  {
    return Command::UNKNOWN;
  }
}

int Host::readDate()
{
  while (true)
  {
    std::cout << "Input date in format DD.MM.YYYY:" << std::endl;
    std::string input;
    std::cin >> input;

    std::istringstream iss(input);

    int output = 0;

    static const int MAX_VALUE[3] = {31, 12, 9999};
    static const int SHIFT[3] = {6, 4, 0};
    try
    {
      for (int i = 0; i < 3; i++)
      {
        std::string word;
        std::getline(iss, word, '.');

        int num = std::stoi(word);
        if (num < 0 || num > MAX_VALUE[i])
          throw std::exception();
        output += num * (int)pow(10, SHIFT[i]);
      }

      if (!iss.eof())
        throw std::exception();

      return output;
    }
    catch (std::exception &e)
    {
      std::cout << "Wrong format, try again:" << std::endl;
    }
  }
} // end of 'Host::readDate' function

void Host::printMenu()
{
  std::cout << std::endl;
  std::cout << "1) Exit" << std::endl;
  std::cout << "2) Get temperature" << std::endl;
  std::cout << "Input command number:" << std::endl;
} // end of 'Host::printMenu' function

void Host::blockAndWait()
{
  const int TIMEOUT_SEC = 5;

  // block self and unblock client
  std::cout << "Blocking self" << std::endl;
  sem_wait(m_host_semaphore);
  std::cout << "Unblocking client" << std::endl;
  sem_post(m_client_semaphore);
  // wait for client response
  struct timespec ts;
  timespec_get(&ts, TIMER_ABSTIME);
  ts.tv_sec += TIMEOUT_SEC;
  if (sem_timedwait(m_host_semaphore, &ts) == -1)
    throw std::runtime_error("Client response wait timeout");
  std::cout << "Response came" << std::endl;
  sem_post(m_host_semaphore);
} // end of 'Host::blockAndWait' function

void Host::close()
{
  m_connection.close();

  if (sem_close(m_host_semaphore) == -1)
    std::cout << "ERROR: failed to close host semaphore, errno = " << strerror(errno) << std::endl;
  if (sem_unlink(HOST_SEMAPHORE_NAME.c_str()) == -1)
    std::cout << "ERROR: failed to unlink host semaphore, errno = " << strerror(errno) << std::endl;
  if (sem_close(m_client_semaphore) == -1)
    std::cout << "ERROR: failed to close client semaphore, errno = " << strerror(errno) << std::endl;
  if (sem_unlink(CLIENT_SEMAPHORE_NAME.c_str()) == -1)
    std::cout << "ERROR: failed to unlink client semaphore, errno = " << strerror(errno) << std::endl;
} // end of 'Host::close' function


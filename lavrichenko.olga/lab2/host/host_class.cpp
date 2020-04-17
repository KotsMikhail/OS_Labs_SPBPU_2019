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

const std::string Host::CONNECTION_NAME = "LAB2_CONN";
const std::string Host::HOST_SEMAPHORE_NAME = "/host_semaphore";
const std::string Host::CLIENT_SEMAPHORE_NAME = "/client_semaphore";
const int Host::EXIT_MSG = -1;

Host::Host()
{
  sem_unlink(HOST_SEMAPHORE_NAME.c_str());
  sem_unlink(CLIENT_SEMAPHORE_NAME.c_str());

  m_host_semaphore = sem_open(HOST_SEMAPHORE_NAME.c_str(), O_CREAT, 0666, 0);
  m_host_semaphore = sem_open(HOST_SEMAPHORE_NAME.c_str(), O_CREAT, 0666, 0);
  if (m_host_semaphore == SEM_FAILED)
  {
    std::ostringstream ss;
    ss << "Failed to open host semaphore, errno = " << strerror(errno);
    throw std::runtime_error(ss.str());
  }

  m_client_semaphore = sem_open(CLIENT_SEMAPHORE_NAME.c_str(), O_CREAT, 0666, 0);
  if (m_client_semaphore == SEM_FAILED)
  {
    std::ostringstream ss;
    ss << "Failed to open client semaphore, errno = " << strerror(errno);
    throw std::runtime_error(ss.str());
  }
}

Host &Host::getInstance()
{
  static Host instance;

  return instance;
}

void Host::start()
{
  std::cout << "Host start" << std::endl;

  sem_post(m_host_semaphore);

  // create client
  switch (fork())
  {       
    case -1:
      throw std::runtime_error("Failed to create client process");
    case 0:
      sleep(1);
      Client(CONNECTION_NAME, m_host_semaphore, m_client_semaphore).start();
      break;
    default:
      std::cout << "Successfully created client process" << std::endl;
      m_connection.open(CONNECTION_NAME, true);
      process();
      close();
      break;
  }
}

void Host::process()
{
  while (true)
  {
    printMenu();
    
    switch (readCommand())
    {
      case Command::COMMAND_COUNT:
      case Command::UNKNOWN:
        std::cout << "Unknown command!" << std::endl;
        break;
      case Command::EXIT:
        std::cout << "Sending 'exit' message to client..." << std::endl;
        m_connection.write(EXIT_MSG);
        blockAndWait();
        std::cout << "Exiting..." << std::endl;
        return;
      case Command::INPUT_DATE:
        int date = readDate();

        m_connection.write(date);
        blockAndWait();

        int temperature = m_connection.read();
        std::cout << "Temperature is " << temperature << " degrees." << std::endl;
        break;
    }
  }
}

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
}

void Host::printMenu()
{
  std::cout << std::endl;
  std::cout << "1) Exit" << std::endl;
  std::cout << "2) Get temperature" << std::endl;
  std::cout << "Input command number:" << std::endl;
}

void Host::blockAndWait()
{
  const int TIMEOUT_SEC = 5;

  sem_wait(m_host_semaphore);
  sem_post(m_client_semaphore);

  struct timespec ts;
  timespec_get(&ts, TIMER_ABSTIME);
  ts.tv_sec += TIMEOUT_SEC;
  if (sem_timedwait(m_host_semaphore, &ts) == -1)
    throw std::runtime_error("Client response wait timeout");
  sem_post(m_host_semaphore);
}

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
}


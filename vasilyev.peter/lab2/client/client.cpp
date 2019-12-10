//
// Created by peter on 11/22/19.
//

#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include "client.h"

Client::Client(const std::string &connectionId, sem_t *host_semaphore,
  sem_t *client_semaphore) :
    m_host_semaphore(host_semaphore),
    m_client_semaphore(client_semaphore)
{
  m_connection.open(connectionId, false);
} // end of 'Client::Client' function

void Client::start()
{
  while (true)
  {
    sem_wait(m_client_semaphore);

    std::cout << "Client unblocked" << std::endl;

    // read message
    int message = m_connection.read();

    std::cout << "Client read message = " << message << std::endl;

    // check if 'exit' message
    if (message == -1)
    {
      std::cout << "Client received 'exit' message" << std::endl;
      // unblock host
      sem_post(m_host_semaphore);
      return;
    }
    // generate temperature
    srand(message);
    int temperature = (int)(((2.0 * rand()) / RAND_MAX - 1.0) * 69);
    std::cout << "Client temperature = " << temperature << std::endl;

    // send temperature
    m_connection.write(temperature);

    std::cout << "Client: unblock host" << std::endl;
    sem_post(m_host_semaphore);
  }
} // end of 'Client::start' function

Client::~Client()
{
  m_connection.close();
} // end of '' function

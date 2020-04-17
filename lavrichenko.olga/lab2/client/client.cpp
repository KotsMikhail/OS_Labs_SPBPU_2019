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
}

void Client::start()
{
  while (true)
  {
    sem_wait(m_client_semaphore);

    int message = m_connection.read();

    if (message == -1)
    {
      std::cout << "Client received 'exit' message" << std::endl;
      sem_post(m_host_semaphore);
      return;
    }
    srand(message);
    int temperature = (int)(((2.0 * rand()) / RAND_MAX - 1.0) * 69);

    m_connection.write(temperature);
    sem_post(m_host_semaphore);
  }
}

Client::~Client()
{
  m_connection.close();
}

#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <string>
#include <semaphore.h>

#include "../interface/conn.h"

class Client
{
private:
  Connection m_connection;
  sem_t *m_host_semaphore;
  sem_t *m_client_semaphore;

public:
  Client(const std::string &connectionId, sem_t *host_semaphore, sem_t *client_semaphore);
  ~Client();
  void start();
};

#endif //LAB2_CLIENT_H

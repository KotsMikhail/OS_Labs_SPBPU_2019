//
// Created by peter on 11/22/19.
//

#ifndef LAB2_HOST_H
#define LAB2_HOST_H

#include <semaphore.h>

#include "../interface/conn.h"

class Host
{
private:
  enum struct Command
  {
    EXIT,
    INPUT_DATE,
    UNKNOWN
  };

  static const std::string CONNECTION_NAME;
  static const std::string HOST_SEMAPHORE_NAME;
  static const std::string CLIENT_SEMAPHORE_NAME;
  static const int EXIT_MSG;

  Connection m_connection;
  sem_t *m_host_semaphore;
  sem_t *m_client_semaphore;

  Host();

  static void printMenu();
  void doWork();
  void blockAndWait();
  void close();

  static Command readCommand();
  static int readDate();

public:
  Host( const Host &host ) = delete;
  Host &operator=( const Host &host ) = delete;

  static Host &getInstance();
  void start();
};

#endif //LAB2_HOST_H

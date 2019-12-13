//
// Created by nero on 24.11.2019.
//
#include "conn.h"
#include <semaphore.h>
#ifndef LAB2_CONNECTION_INFO_H
#define LAB2_CONNECTION_INFO_H
struct connection_info
{
   conn* connection;
   sem_t* semaphore;
   int id;
   connection_info(conn* c, sem_t* s, int id) : connection(c), semaphore(s), id(id){ }
};
#endif //LAB2_CONNECTION_INFO_H

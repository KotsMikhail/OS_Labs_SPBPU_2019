//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>

#include "memory.h"
#define SEM_NAME "host_client_sem"
#define TIMEOUT 5

class Conn
{
public:
    bool Open(size_t id, bool create);
    bool Read(void *buf, size_t count = sizeof(Memory));
    bool Write(void *buf, size_t count = sizeof(Memory));
    bool Close();
};

#endif //LAB2_CONN_H

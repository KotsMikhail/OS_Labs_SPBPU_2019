//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>
#include "message.h"

#define CLIENT_SEM_NAME "client_sem"
#define HOST_SEM_NAME "host_sem"
const int TIMEOUT = 5;

class Conn
{
public:
    bool Open(size_t id, bool create);
    bool Read(void *buf, size_t count = sizeof(Message));
    bool Write(void *buf, size_t count = sizeof(Message));
    bool Close();
};

#endif //LAB2_CONN_H

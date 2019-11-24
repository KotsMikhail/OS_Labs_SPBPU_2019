//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>
#include "message.h"
#include <errno.h>

#define CLIENT_SEM_NAME "client_sem"
#define HOST_SEM_NAME "host_sem"
const int TIMEOUT = 5;

class Conn
{
public:
    Conn(bool is_host) : is_host(is_host) {}
    bool Open(size_t id, bool create);
    bool Read(void *buf, size_t count = sizeof(Message));
    bool Write(void *buf, size_t count = sizeof(Message));
    bool Close();
private:
    bool is_host;
};

#endif //LAB2_CONN_H

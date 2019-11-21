//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>

#define SEM_NAME "host_client_sem"

class Conn
{
public:
    Conn(size_t id , bool create);
    bool Read(void *buf, size_t count);
    bool Write(void *buf, size_t count);
    ~Conn();
};

#endif //LAB2_CONN_H

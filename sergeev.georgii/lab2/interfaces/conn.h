//
// Created by nero on 22.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H
#include <cstdlib>

#include "memory.h"
#include "Message.h"
#include <string>
class conn
{
public:
    void Open(size_t id);
    void Read(size_t id, void *buf, size_t count = sizeof(message));
    void Write(size_t id, void *buf, size_t count = sizeof(message));
    void Close(size_t id);
};
#endif //LAB2_CONN_H

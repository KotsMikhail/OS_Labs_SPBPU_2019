//
// Created by nero on 22.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H
#include <cstdlib>

#include "Message.h"
#include <string>
class conn
{
public:
    bool Open(size_t id);
    bool Read(void *buf, size_t count = sizeof(message));
    bool Write(void *buf, size_t count = sizeof(message));
    void Close();
private:
    int *fd;
    std::string name;

};
#endif //LAB2_CONN_H

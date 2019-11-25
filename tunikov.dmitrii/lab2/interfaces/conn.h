//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>
#include "message.h"
#include <errno.h>

const int TIMEOUT = 5;

class Conn
{
public:
    Conn();
    bool Open(size_t id, bool create);
    bool Read(void *buf, size_t count = sizeof(Message));
    bool Write(void *buf, size_t count = sizeof(Message));
    bool Close();
private:
    bool is_host;
    static const char* channel_name;
    static bool is_created;
    static int desc;
    static int listener;
};

#endif //LAB2_CONN_H

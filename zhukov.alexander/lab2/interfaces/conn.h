#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>

#include "memory.h"
#include "message.h"
#include <string>

class conn {
public:
    void Open(size_t id);

    void Read(void *buf, size_t count = sizeof(message));

    void Write(void *buf, size_t count = sizeof(message));

    void Close();
private:
    int* fd;
    const char* name;
};

#endif //LAB2_CONN_H

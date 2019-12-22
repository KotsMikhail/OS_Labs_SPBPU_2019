#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>

#include "memory.h"
#include "message.h"
#include <string>

class conn {
public:
    bool Open(size_t id);

    bool Read(void *buf, size_t count = sizeof(message));

    bool Write(void *buf, size_t count = sizeof(message));

    void Close();
private:
    int* fd;
    std::string name;
};

#endif //LAB2_CONN_H

#ifndef CONN_H
#define CONN_H

#include <sys/types.h>
#include "message.h"

const unsigned int TIMEOUT_SEC = 5;
#define SEMAPHORE_HOST "semaphore host"
#define SEMAPHORE_CLIENT "semaphore client"

class conn_t
{
public:
    int create_connection(bool isHost, bool create);
    int read(message_t *buf, size_t size);
    int write(message_t *buf, size_t size);
    int destroy_connection();
private:
    bool is_owner;
    int id;
};



#endif
#ifndef __CONN_H__
#define __CONN_H__

#include <cstdlib>
#include <errno.h>

#include "message.h"

const char * const HOST_SEM_NAME   = "/lab2_host_sem";
const char * const CLIENT_SEM_NAME = "/lab2_client_sem";

class Conn
{
public:
    Conn  ();
    ~Conn ();

    bool Open  (size_t id, bool create);
    bool Read  (void *buf, size_t count = sizeof(Message));
    bool Write (void *buf, size_t count = sizeof(Message));
    bool Close ();

private:
    bool isHost, wasCreated;
    int descID;
    int *additionalArgs;
};

#endif //__CONN_H__

#ifndef CONNECT_H
#define CONNECT_H

#include <stdlib.h>

class conn_t
{
public:
    conn_t();
    ~conn_t();

    bool conn_open(size_t id, bool create);
    bool conn_close();
    
    bool conn_recv(void *buf, size_t count);
    bool conn_send(void *buf, size_t count);

private:
    bool _need_to_rm, _is_open;
    int *_desc;
};

#endif // CONNECT_H

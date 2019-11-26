//
// Created by misha on 26.11.2019.
//

#ifndef LAB_2_SEVER__SERVER_CONN_H
#define LAB_2_SEVER__SERVER_CONN_H

#include <stdlib.h>
#include <string>

class Server_conn {

public:
    Server_conn() = default;
    ~Server_conn() = default;

    bool Create();
    bool Open(size_t id);
    bool Close();
    bool Read(void *buf, size_t count);
    bool Write(void *buf, size_t count);
    int GetClientID();

private:
    bool owner;
    int client_pid;
    int fd[2];

};


#endif //LAB_2_SEVER__SERVER_CONN_H

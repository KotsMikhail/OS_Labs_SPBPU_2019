//
// Created by misha on 23.11.2019.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <string>
#include <bits/types/siginfo_t.h>
#include <vector>
#include "Server_conn.h"
#include "../interfaces/message.h"


class Server {
public:
    static Server* GetInstance();
    void Start();
private:
    Server();
    //Server(Server&);
    void Terminate(int signum);
    static void SignalHandler(int signum, siginfo_t* info, void* ptr);

    std::vector<Server_conn> pipes;
    void SendTask(Message mes);
    void OpenConnection(Server_conn new_connection);
    void CloseConnection(int pid);

};


#endif //SERVER_SERVER_H

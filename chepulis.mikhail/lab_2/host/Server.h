//
// Created by misha on 23.11.2019.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <string>
#include <vector>
#include "Server_conn.h"
#include "../interfaces/message.h"
#include "ClientHandler.h"
#include <signal.h>



class Server {
public:
    static Server *GetInstance();

    void Start();

private:

    Server();

    Server(Server &other);

    Server &operator=(Server &other);

    void Terminate(int signum);

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);

    std::vector<Server_conn> pipes;
    std::vector<ClientHandler *> handlers;

    void SendTask(Message mes);

    void OpenConnection(Server_conn new_connection, ClientHandler *);

    void CloseConnection(int pid);

    void Run();

};


#endif //SERVER_SERVER_H

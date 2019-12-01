//
// Created by misha on 23.11.2019.
//

#include "Server.h"
#include <iostream>
#include <sys/stat.h>
#include <cstdio>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <syslog.h>
#include <wait.h>
#include "../interfaces/message.h"
#include "ClientHandler.h"
#include "Oracle.h"
#include "my_functions.h"






Server* Server::GetInstance() {
    static Server self;
    return &self;
}


Server::Server() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
}

Server::Server(Server& other){}
Server& Server::operator=(Server& other) {
    return other;
}


void* routine(void * arg) {
    ClientHandler *handler = (ClientHandler *) arg;
    handler->Start();
    return nullptr;
}

void Server::Start() {
    //std::cout << "I am server!" << std::endl;
    syslog(LOG_NOTICE, "I am server!");

    Message buf;
    Message msg;
    msg.number = 0;


    int client_count = 1;
    std::cout << "Server`s pid is " << getpid() << std::endl;
    std::cout << "Enter number of Weather Prophets" << std::endl;
    std::cin >> client_count;
    while (std::cin.fail() || client_count <= 0) {
        std::cin.clear(); // то возвращаем cin в 'обычный' режим работы
        std::cin.ignore(32767, '\n'); // и удаляем значения предыдущего ввода из входного буфера
        std::cout << "Wrong number.Try again. Number should be > 0 " << std::endl;
        std::cin >> client_count;
    }
    std::cout << "Number = " << client_count << std::endl;

    int handler_id, client_pid;
    Server_conn new_connection;

    for (int i = 0; i < client_count; i++) {
        new_connection.Create();

        ClientHandler *handler = new ClientHandler(i);
        handler->SetServerConnection(new_connection);
        handler->OpenConnection();
        handler_id = handler->GetId();

        client_pid = fork();
        if (client_pid == 0) {
            Oracle *client = Oracle::GetInstance(handler_id);
            client->SetPipe(handler->GetPipe());
            client->OpenConnection();
            client->Start();
            //std::cout << "client work is over" << std::endl;
            syslog(LOG_NOTICE, "client work is over");
            return;
        }
        handler->SetClient(client_pid);

        new_connection.Open(handler_id);


        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        void *(*function)(void *) = routine;
        pthread_create(&tid, &attr, function, handler);
        handler->SetTID(tid);
        OpenConnection(new_connection, handler);
    }

    std::string new_date;
    Message mes;
    bool flag;
    while (true) {
        new_date = "";
        flag = true;

        while (!CheckDate(new_date)) {


            if (std::cin.fail()) {
                std::cin.clear(); // то возвращаем cin в 'обычный' режим работы
                //std::cin.ignore(32767, '\n'); // и удаляем значения предыдущего ввода из входного буфера
                //std::cout << "clear cin" << std::endl;
                new_date = "";
            } else {
                if (flag) {
                    flag = false;
                } else {
                    std::cout << "Wrong date. Try again. Format: DD.MM.YYYY" << std::endl;
                }
                std::cout << "Enter the date. Format: DD.MM.YYYY" << std::endl << "Or enter \"q\" for exit"
                          << std::endl;
            }
            std::cin >> new_date;
            if (new_date == "q" || new_date == "Q") {
                std::cout << "Close Weather-Prophet server" << std::endl;
                Terminate(EXIT_SUCCESS);
            }
        }

        MessFromDate(new_date, mes);
        //std::cout << "Server (" << getpid() << ") send Task " << new_date << std::endl;
        SendTask(mes);
        //sleep(1);
    }

}



void Server::Terminate(int signum) {
    //std::cout << "Server::Terminate()" << std::endl;
    syslog(LOG_NOTICE, "Server::Terminate()");
    //TODO уничтожить всех потоки общения с клиентом

    //std::cout << "pipes.size = " << pipes.size() << std::endl;
    for (unsigned int i = 0; i < pipes.size(); i++) {
        //std::cout << "(" << getpid() << ") kill Handler : " << pipes[i].GetClientID() << std::endl;
        CloseConnection(i);
    }

    //std::cout << "server terminate : " << std::endl;
    //std::cout << "pipes.size = " << pipes.size() << std::endl;

    /*
    for (unsigned int i = 0; i < pipes.size(); i++) {
        std::cout << i <<") " << pipes[i].GetClientID() << " :" << pipes[i].IsClosed() <<std::endl;
    }
    */
    //sleep(5);
    closelog();
    exit(signum);

}


void Server::SignalHandler(int signum, siginfo_t* info, void* ptr){
    static Server* instance = GetInstance();
    switch (signum)
    {
        case SIGTERM:
        {
            instance->Terminate(signum);
            break;
        }
    }
}


void Server::SendTask(Message  mes) {
    for(unsigned int i = 0; i < pipes.size(); i++)
    {
        //std::cout << "sendTask (" << pipes[i].GetClientID()  << ")" << std::endl;
        if(!pipes[i].IsClosed()) {
            pipes[i].Write((void *) &mes, sizeof(mes));
        }
    }
}


void Server::OpenConnection(Server_conn new_connection, ClientHandler* handler) {
    pipes.push_back(new_connection);
    handlers.push_back(handler);
}


void Server::CloseConnection(int pid) {
    for (unsigned int i = 0; i < pipes.size(); i++) {
        if (pipes[i].GetClientID() == pid && !pipes[i].IsClosed()) {
            //std::cout << "close server connection (" << pid << ")" << std::endl;
            syslog(LOG_NOTICE, "close server connection (%i)", pid);
            pipes[i].Close();
            pthread_cancel(handlers[i]->GetTID());
            delete (handlers[i]);
            return;
        }
    }
}









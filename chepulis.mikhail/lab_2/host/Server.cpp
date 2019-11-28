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
#include <semaphore.h>
#include <syslog.h>
#include <wait.h>
#include "../interfaces/message.h"
#include "ClientHandler.h"
#include "Oracle.h"
#include "my_functions.h"








Server* Server::GetInstance(){
    static Server self;
    return &self;
}


Server::Server(){
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGCLD, &act, nullptr);


}

void Server::Start() {
    //std::cout << "I am server!" << std::endl;
    syslog(LOG_NOTICE, "I am server!");


    Message buf;
    Message msg;
    msg.number = 0;
    int handler_pid, client_pid;


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

    Server_conn new_connection;
    for (int i = 0; i < client_count; i++) {
        new_connection.Create();

        handler_pid = fork();
        if (handler_pid == 0) {
            ClientHandler *handler = ClientHandler::GetInstance();
            handler->SetServerConnection(new_connection);
            handler->OpenConnection();
            handler_pid = getpid();
            client_pid = fork();
            if (client_pid == 0) {
                Oracle *client = Oracle::GetInstance(handler_pid);
                client->SetPipe(handler->GetPipe());
                client->OpenConnection();
                client->Start();
                //std::cout << "client work is over" << std::endl;
                syslog(LOG_NOTICE, "client work is over");
                return;
            }
            handler->SetClient(client_pid);
            handler->Start();
            return;
        }

        new_connection.Open(handler_pid);
        OpenConnection(new_connection);
    }


    std::string new_date = "25.11.2019";
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
                std::cout << "Enter the date. Format: DD.MM.YYYY" << std::endl << "Or enter \"q\" for exit" << std::endl;
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
    //syslog(LOG_NOTICE, "Server::Terminate()" );
    //TODO уничтожить всех потоки общения с клиентом
    for(unsigned int i = 0; i < pipes.size(); i++)
    {
        //std::cout << "kill Handler : " << i << std::endl;
        //syslog(LOG_NOTICE, "kill Handler : (%d)", i);
        kill(pipes[i].GetClientID(), SIGTERM);

    }
    for(unsigned int i = 0; i < pipes.size(); i++)
    {
        CloseConnection(pipes[i].GetClientID());
    }


    closelog();
    exit(signum);

}


void Server::SignalHandler(int signum, siginfo_t* info, void* ptr){
    static Server* instance = GetInstance();
    switch (signum)
    {
        case SIGCLD:
        {
            //std::cout << "son closed (" << info->si_pid  << ")" << std::endl;
            syslog(LOG_NOTICE, "son closed (%d)", info->si_pid);
            instance->CloseConnection(info->si_pid);
            break;
        }
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
        pipes[i].Write((void*)&mes, sizeof(mes));
    }
}

void Server::OpenConnection(Server_conn new_connection) {
    pipes.push_back(new_connection);
}

void Server::CloseConnection(int pid) {
    for(unsigned int i = 0; i < pipes.size(); i++)
    {
        if(pipes[i].GetClientID() == pid)
        {
            //std::cout << "close server connection (" << pid  << ")" << std::endl;
            syslog(LOG_NOTICE, "close server connection (%i)", pid);
            pipes[i].Close();
            pipes.erase(pipes.begin() + i);
        }
    }
}






//
// Created by misha on 24.11.2019.
//

#include "ClientHandler.h"
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <semaphore.h>
#include <syslog.h>
#include "../interfaces/message.h"
#include "my_functions.h"



ClientHandler* ClientHandler::GetInstance(){
    static ClientHandler self;
    return &self;
}


ClientHandler::ClientHandler(): client_info(0){
    signal(SIGTERM, SignalHandler);
}

ClientHandler::ClientHandler(ClientHandler&): client_info(0){}


bool ClientHandler::OpenConnection(){
    if (connection.Open(getpid(), true))
    {
        semaphore_name = sem_name + "_" +  toString(getpid());
        sem_client_name = sem_name +"_client_" + toString(getpid());
        sem_host_name = sem_name + "_host_" + toString(getpid());

        semaphore = sem_open(semaphore_name.c_str(), O_CREAT, 0666, 1);
        if (semaphore == SEM_FAILED)
        {
            //std::cout << "ERROR: sem_open failed with error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: sem_open failed with error = %s", strerror(errno));
            return false;
        }
        //std::cout << "handler: semaphore created (" << semaphore_name << ")" << std::endl;
        syslog(LOG_NOTICE, "handler: semaphore created (%s)", semaphore_name.c_str());

        semaphore_host = sem_open(sem_host_name.c_str(), O_CREAT, 0666, 0);
        if(semaphore_host == SEM_FAILED){
            //std::cout << "ERROR: handler: can`t open host semaphore error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: handler: can`t open host semaphore error = %s", strerror(errno));
            return false;
        }
        //std::cout << "handler: host semaphore created (" << sem_host_name << ")" << std::endl;
        syslog(LOG_NOTICE, "handler: host semaphore created (%s)", semaphore_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), O_CREAT, 0666, 0);
        if(semaphore_client == SEM_FAILED){
            //std::cout << "ERROR: handler: can`t open client semaphore error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: handler: can`t open client semaphore error = %s", strerror(errno));
            return false;
        }
        //std::cout << "handler: client semaphore created (" << sem_client_name << ")" << std::endl;
        syslog(LOG_NOTICE, "handler: client semaphore created (%s)", semaphore_name.c_str());
        //std::cout << "handlers pid: " << getpid() << std::endl;
        syslog(LOG_NOTICE, "handlers pid: %d", getpid());
        return true;
    }
    return false;
}




void ClientHandler::Start() {
    //std::cout << "I am handler! pid: " << getpid() << std::endl;
    syslog(LOG_NOTICE, "I am handler!");

    struct timespec ts;
    Message buf;
    Message mes;
    int s;
    while (true) {

        if(!GetTask(mes))
        {
            //std::cout << "Task yet done\n";
            sleep(1);
            continue;
        }
        if (connection.Write(&mes, sizeof(mes))) {

            sem_post(semaphore_client);
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            while ((s = sem_timedwait(semaphore_host, &ts)) == -1 && errno == EINTR)
                continue;
            if (s == -1) {
                //TODO "убить" себя и клиента
                //std::cout << "KILL KILL KILL KILL KILL KILL KILL" << std::endl;
                syslog(LOG_NOTICE, "KILL KILL KILL KILL KILL KILL KILL");
                KillClient();
                Terminate(errno);
            } else {
                if (connection.Read(&buf, sizeof(buf))) {
                    std::cout << getpid() << "|" << client_info.pid <<  "\t:\tdate = " << mes.day << "." << mes.month << "." << mes.year;
                    std::cout << "\ttemperature = " << buf.number << "\n";
                }
            }
        }
    }
}


void ClientHandler::Terminate(int signum) {
    //std::cout << "Handler::Terminate(" << getpid() << ")" << std::endl;
    syslog(LOG_NOTICE, "Handler::Terminate(%d)", getpid());
    KillClient();

    if (!connection.Close())
    {
        //std::cout << "Terminating error connection: " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "Terminating error connection: %s", strerror(errno));
    }

    if (sem_unlink(sem_host_name.c_str()) != 0)
    {
        //std::cout << "Terminating error sem_host_name: " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "Terminating error sem_host_name: %s", strerror(errno));
    }

    if (sem_unlink(sem_client_name.c_str()) != 0)
    {
        //std::cout << "Terminating error sem_client_name: " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "Terminating error sem_client_name: %s", strerror(errno));
    }
    //std::cout << "handler was killed (" << getpid() << ")" << std::endl;
    syslog(LOG_NOTICE, "handler was killed (%d)", getpid());
    exit(signum);

}


void ClientHandler::SignalHandler(int signum){
    ClientHandler* instance = GetInstance();
    switch (signum)
    {
        case SIGTERM:
        {
            //std::cout << "SEGTERM for handler (" << getpid() << ")"  << std::endl;
            syslog(LOG_NOTICE, "SEGTERM for handler (%d)", getpid());
            instance->Terminate(signum);
        }
    }
}

void ClientHandler::SetClient(int client_pid) {
    //std::cout << "Set Client:" << client_pid << std::endl;
    syslog(LOG_NOTICE, "Set Client: (%d)", client_pid);
    client_info = ClientInfo(client_pid);
}

Conn ClientHandler::GetPipe() {
    return connection ;
}

void ClientHandler::KillClient() {

    if (client_info.attached)
    {
        //std::cout << "Kill client" << std::endl;
        syslog(LOG_NOTICE, "Kill client");
        kill(client_info.pid, SIGTERM);
    }
    client_info = ClientInfo(0);
}


void ClientHandler::SetServerConnection(Server_conn serv_conn_) {
    serv_con = serv_conn_;
    serv_con.Open(getpid());
}

bool ClientHandler::GetTask(Message &mes) {
    Message buf;
    //std::cout << "GetTask (" << getpid() << ")" << std::endl;
    if(serv_con.Read(&buf, sizeof(buf)))
    {
        mes = buf;
        //std::cout << "date = " << mes.day << "." << mes.month << "." << mes.year << std::endl;
        return true;
    }
    return false;
}




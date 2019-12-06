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




ClientHandler::~ClientHandler() {
    Terminate(EXIT_SUCCESS);
}

ClientHandler::ClientHandler(int id_): client_info(0), my_id(id_) {
}

ClientHandler::ClientHandler(ClientHandler&): client_info(0), is_already_closed(false) {
}

bool ClientHandler::OpenConnection() {
    if (connection.Open(my_id, true)) {
        sem_client_name = sem_name + "_client_" + std::to_string(my_id);
        sem_host_name = sem_name + "_host_" + std::to_string(my_id);


        semaphore_host = sem_open(sem_host_name.c_str(), O_CREAT, 0666, 0);
        if (semaphore_host == SEM_FAILED) {
            //std::cout << "ERROR: handler: can`t open host semaphore error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: handler: can`t open host semaphore error = %s", strerror(errno));
            return false;
        }
        //std::cout << "handler: host semaphore created (" << sem_host_name << ")" << std::endl;
        syslog(LOG_NOTICE, "handler: host semaphore created (%s)", sem_host_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), O_CREAT, 0666, 0);
        if (semaphore_client == SEM_FAILED) {
            //std::cout << "ERROR: handler: can`t open client semaphore error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: handler: can`t open client semaphore error = %s", strerror(errno));
            sem_unlink(sem_host_name.c_str());
            return false;
        }
        //std::cout << "handler: client semaphore created (" << sem_client_name << ")" << std::endl;
        syslog(LOG_NOTICE, "handler: client semaphore created (%s)", sem_client_name.c_str());
        //std::cout << "handlers pid: " << my_id << std::endl;
        syslog(LOG_NOTICE, "handlers pid: %d", my_id);
        return true;
    }
    return false;
}




void ClientHandler::Start() {
    //std::cout << "I am handler! pid: " << my_id << std::endl;
    syslog(LOG_NOTICE, "I am handler!");

    struct timespec ts;
    Message buf;
    Message mes;
    int s;
    while (true) {

        if (!GetTask(mes)) {
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
                // если клиент завис, то "убиваемм" клиента и закрываем соединение
                KillClient();
                Terminate(errno);
                return;
            } else {
                if (connection.Read(&buf, sizeof(buf))) {
                    std::cout << my_id << "|" << client_info.pid << "\t:\tdate = " << mes.day << "." << mes.month << "."
                              << mes.year;
                    std::cout << "\ttemperature = " << buf.number << "\n";
                }
            }
        }
    }
}


void ClientHandler::Terminate(int signum) {
    if (is_already_closed)
        return;
    is_already_closed = true;
    //std::cout << "Handler::Terminate(" << my_id << ")" << std::endl;
    syslog(LOG_NOTICE, "Handler::Terminate(%d)", my_id);
    KillClient();

    if (!connection.Close()) {
        std::cout << "Terminating error connection: " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "Terminating error connection: %s", strerror(errno));
    }
    if (semaphore_host != SEM_FAILED) {
        semaphore_host = SEM_FAILED;
        if (sem_unlink(sem_host_name.c_str()) != 0) {
            std::cout << "Terminating error sem_host_name: " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "Terminating error sem_host_name: %s", strerror(errno));
        }
    }

    if (semaphore_client != SEM_FAILED) {
        semaphore_client = SEM_FAILED;
        if (sem_unlink(sem_client_name.c_str()) != 0) {
            std::cout << "Terminating error sem_client_name: " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "Terminating error sem_client_name: %s", strerror(errno));
        }
    }
    //std::cout << "handler was killed (" << my_id << ")" << std::endl;
    syslog(LOG_NOTICE, "handler was killed (%d)", my_id);

    //exit(signum);

}


void ClientHandler::SetClient(int client_pid) {
    //std::cout << "Set Client:" << client_pid << std::endl;
    syslog(LOG_NOTICE, "Set Client: (%d)", client_pid);
    client_info = ClientInfo(client_pid);
}

Conn ClientHandler::GetPipe() {
    return connection;
}


void ClientHandler::KillClient() {

    if (client_info.attached) {
        //std::cout << "Kill client" << std::endl;
        syslog(LOG_NOTICE, "Kill client");
        kill(client_info.pid, SIGTERM);
    }
    client_info = ClientInfo(0);
}


void ClientHandler::SetServerConnection(Server_conn serv_conn_) {
    serv_con = serv_conn_;
    serv_con.Open(my_id);
}


bool ClientHandler::GetTask(Message &mes) {
    Message buf;
    //std::cout << "GetTask (" << my_id << ")" << std::endl;
    if (serv_con.Read(&buf, sizeof(buf))) {
        mes = buf;
        //std::cout << "date = " << mes.day << "." << mes.month << "." << mes.year << std::endl;
        return true;
    }
    return false;
}


int ClientHandler::GetId() {
    return my_id;
}


void ClientHandler::SetTID(pthread_t tid_) {
    tid = tid_;
}

pthread_t ClientHandler::GetTID() {
    return tid;
}







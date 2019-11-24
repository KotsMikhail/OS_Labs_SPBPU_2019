//
// Created by dmitrii on 23.11.2019.
//

#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <semaphore.h>
#include <conn.h>
#include <csignal>

class Client
{
public:
    static Client& getInstance(int host_pid);
    void run();
    bool openConnection();
private:
    Conn connection;
    sem_t* host_semaphore{};
    sem_t* client_semaphore{};
    int host_pid;

    explicit Client(int host_pid);
    void close(int signum);
    static int getRandTemp(const Date& date);
    static void signalHandler(int sig, siginfo_t *info, void *context);
};


#endif //LAB2_CLIENT_H

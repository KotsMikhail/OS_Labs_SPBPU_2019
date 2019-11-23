//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_HOST_H
#define LAB2_HOST_H
#include <semaphore.h>
#include <csignal>
#include <conn.h>
#include "client_data.h"

class Host
{
public:
    static Host& getInstance();
    void run();
    bool openConnection();
private:
    sem_t* semaphore{};
    Conn connection;
    ClientData client_data;
    static void signalHandler(int sig, siginfo_t* info, void* ptr);
    bool checkTimeout(Memory& mem);
    static Date getRandDate();
    static void close(int exit_code);

    Host();
};

#endif //LAB2_HOST_H

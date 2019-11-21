//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_HOST_H
#define LAB2_HOST_H
#include <semaphore.h>
#include "../../interfaces/conn.h"
#include "client_data.h"

class Host
{
public:
    static Host& getInstance();
    void run();
private:
    sem_t* semaphore;
    Conn connection;
    static ClientData client_data;
    static void signalHandler(int sig, siginfo_t* info, void* ptr);

    std::string getRandDate();

    Host();
    ~Host();
};

#endif //LAB2_HOST_H

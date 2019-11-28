//
// Created by misha on 23.11.2019.
//

#ifndef LAB_2_SEVER__ORACLE_H
#define LAB_2_SEVER__ORACLE_H


#include "../interfaces/message.h"
#include "semaphore.h"
#include "../interfaces/Conn.h"

class Oracle {
public:
    static Oracle* GetInstance(int host_pid);
    void Start();
    bool OpenConnection();
    void SetPipe(Conn connection_);
private:
    Conn connection;
    sem_t* semaphore_host;
    sem_t* semaphore;
    sem_t* semaphore_client;
    std::string semaphore_name;
    std::string sem_client_name;
    std::string sem_host_name;

    int host_pid;
    int rand_offset;

    Oracle(int host_pid);
    Oracle(Oracle& other);
    void Terminate(int signum);
    int GetWeather(int day, int month, int year);
    static void SignalHandler(int signum);
};


#endif //LAB_2_SEVER__ORACLE_H

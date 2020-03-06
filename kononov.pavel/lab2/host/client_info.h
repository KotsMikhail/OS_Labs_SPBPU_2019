#ifndef OS_LABS_SPBPU_2019_CLIENT_INFO_H
#define OS_LABS_SPBPU_2019_CLIENT_INFO_H


#include <conn.h>
#include <semaphore.h>
#include <message.h>
#include <cstring>


struct ClientInfo
{
    int pid;
    bool attached;
    int count_dead;
    int id;
    Conn connection;
    sem_t* semaphore_host;
    sem_t* semaphore_client;

    explicit ClientInfo(int pid = 0, int id = 0) : pid(pid), attached(pid != 0), count_dead(0), id(id),
                                   semaphore_host(nullptr), semaphore_client(nullptr)
    {
    }
    bool OpenConnection(int i);
    void Attach(int pid);
    void Delete();
    void Dettach();
};

#endif //OS_LABS_SPBPU_2019_CLIENT_INFO_H

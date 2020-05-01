#ifndef OS_LABS_SPBPU_2019_CLIENT_INFO_H
#define OS_LABS_SPBPU_2019_CLIENT_INFO_H


#include <conn.h>
#include <semaphore.h>
#include <message.h>
#include <cstring>


class ClientInfo {
private:
    int pid;
    bool attached;
    int id;
    Conn connection;
    sem_t *semaphore_host;
    sem_t *semaphore_client;

public:
    bool IsAttached();

    int GetId();

    int GetPid();

    Conn &GetConnection();

    sem_t &GetHostSemaphore();

    sem_t &GetClientSemaphore();

    explicit ClientInfo(int pid = 0, int id = 0) : pid(pid), attached(pid != 0), id(id), 
                                                   semaphore_host(nullptr), semaphore_client(nullptr), count_dead(0) {
    }

    bool OpenConnection(int i);

    void Attach(int pid);

    void Delete();

    void Dettach();


    int count_dead;
};

#endif //OS_LABS_SPBPU_2019_CLIENT_INFO_H

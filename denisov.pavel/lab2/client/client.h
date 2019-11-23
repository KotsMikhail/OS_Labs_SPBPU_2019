#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <conn.h>
#include <semaphore.h>
#include <bits/types/siginfo_t.h>

class Client
{
public:
    static Client& GetInstance (pid_t pid);

    void Start     ();
    void Terminate ();

    bool OpenConnection ();

private:
    const int MIN_RAND       = 1;
    const int ALIVE_MAX_RAND = 100;
    const int DEAD_MAX_RAND  = 50;

    pid_t hostPid;
    Conn conn;

    sem_t *semaphore_host, *semaphore_client;

    Client (pid_t pid);

    int  GetNewRandomNumber (int rightRandEdge);
};

#endif //__CLIENT_H__

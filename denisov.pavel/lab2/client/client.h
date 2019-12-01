#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <conn.h>
#include <semaphore.h>
#include <csignal>

class Client
{
public:
    Client () = delete;
    Client (const Client&) = delete;
    Client & operator=(const Client&) = delete;

    static Client& GetInstance (pid_t pid);

    void Start     ();
    void Terminate ();

    bool OpenConnection ();

private:
    static const int MIN_RAND       = 1;
    static const int DEAD_MAX_RAND  = 50;
    static const int ALIVE_MAX_RAND = 100;

    pid_t hostPid;
    Conn conn;

    sem_t *semaphore_host, *semaphore_client;

    Client (pid_t pid);

    static int GetNewRandomNumber (int rightRandEdge);
};

#endif //__CLIENT_H__

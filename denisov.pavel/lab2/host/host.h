#ifndef __HOST_H__
#define __HOST_H__

#include <conn.h>
#include <message.h>
#include <csignal>
#include <semaphore.h>

class Host
{
public:
    Host (const Host&) = delete;
    Host & operator=(const Host&) = delete;

    static Host& GetInstance ();

    void  Start            ();
    bool  IsClientAttached ();
    void  AttachNewClient  (pid_t clientPid);
    pid_t GetClientPid     ();
    void  Terminate        ();
    void  DeattachClient   ();

    bool OpenConnection    ();

private:
    struct Client
    {
        pid_t pid;
        bool isAttached;

        int numOfFails;

        Client ()
        : pid(0)
        , isAttached(false)
        , numOfFails(0)
        {
        }

        Client (pid_t newPid)
        : pid(newPid)
        , isAttached(newPid != 0)
        , numOfFails(0)
        {
        }
    };

private:
    Client curClientInfo;
    Conn   conn;
    sem_t *semaphore_host, *semaphore_client;

    int curNumber;

    Host ();
    void SendFirstMessageToClient ();
    Message CountClientStatus     (const Message& curClientMessage);
};

#endif //__HOST_H__

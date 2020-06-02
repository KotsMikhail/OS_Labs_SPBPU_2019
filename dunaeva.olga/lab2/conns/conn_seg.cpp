#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <cstring>

#include "conn.h"

using namespace std;

void Conn::ReportError(const string &msg)
{
    syslog(LOG_ERR, "%s", msg.c_str());
    kill(pid, SIGTERM);
    exit(EXIT_FAILURE);
}

Conn::Conn(pid_t p, bool c)
{
    pid = p;
    create = c;

    int shmflg = S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH;

    if (create)
        shmflg |= IPC_CREAT;
    
    fileDesc[0] = shmget(pid, sizeof(int), shmflg);

    if (fileDesc[0] == -1)
        ReportError("Shmget error.");
    
    if (create)
        syslog(LOG_INFO, "Host opened a connection.");
    else
        syslog(LOG_INFO, "Client connected to connection.");
}

void Conn::Write(int num)
{
    void *shmaddr = shmat(fileDesc[0], nullptr, 0);
    
    if (shmaddr == (void*)(-1))
        ReportError("Shmat error.");

    memcpy(shmaddr, (const char*)(&num), sizeof(int));

    if (shmdt(shmaddr) == -1)
        ReportError("Shmdt error.");
}

int Conn::Read()
{
    int num;

    void *shmaddr = shmat(fileDesc[0], nullptr, 0);
    
    if (shmaddr == (void*)(-1))
        ReportError("Shmat error.");

    memcpy((char*)(&num), shmaddr, sizeof(int));

    if (shmdt(shmaddr) == -1)
        ReportError("Shmdt error.");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        if (shmctl(fileDesc[0], IPC_RMID, nullptr) == -1)
            ReportError("Shmctl error.");
    
        syslog(LOG_INFO, "Host closed a connection.");
        syslog(LOG_INFO, "Host completed.");
    }
}

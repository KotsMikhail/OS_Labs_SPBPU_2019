#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <cstdlib>
#include <string>

#include "wolf.h"
#include "little_goat.h"

using namespace std;

const string SHM_HOST = "shm_host";
const string SHM_CLIENT = "shm_client";

const int TIMEOUT = 5;

void ReportError(const string &msg)
{
    syslog(LOG_ERR, "%s", msg.c_str());
    exit(EXIT_FAILURE);
}

int OpenShm(const string &shmName)
{
    int shm = shm_open(shmName.c_str(), O_RDWR | O_CREAT, S_IRWXU);

    if (shm == -1)
        ReportError("Shm_open error.");

    return shm;
}

sem_t* InitSem(int shm)
{
    if (ftruncate(shm, sizeof(sem_t)) == -1)
        ReportError("Ftruncate error.");
    
    sem_t *sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    if (sem == MAP_FAILED)
        ReportError("Mmap error.");
    
    if (sem_init(sem, 1, 0) == -1)
        ReportError("Sem_init error.");
    
    return sem;
}

void PostSem(sem_t *sem)
{
    if (sem_post(sem) == -1)
        ReportError("Sem_post error.");
}

void WaitSemTimed(sem_t *sem)
{
    struct timespec ts;
    
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        ReportError("Clock_gettime error.");
    
    ts.tv_sec += TIMEOUT;

    if (sem_timedwait(sem, &ts) == -1)
        ReportError("Sem_timedwait error.");
}

void WaitSem(sem_t *sem)
{
    if (sem_wait(sem) == -1)
        ReportError("Sem_wait error.");
}

void DestroySem(sem_t *sem)
{
    if (sem_destroy(sem) == -1)
        ReportError("Sem_destroy error.");
}

void UnlinkShm(const string &shmName)
{
    if (shm_unlink(shmName.c_str()) == -1)
        ReportError("Shm_unlink error.");
}

int main()
{
    openlog("game", LOG_CONS | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Host run.");

    int shmHost = OpenShm(SHM_HOST), shmClient = OpenShm(SHM_CLIENT);
    sem_t *semHost = InitSem(shmHost), *semClient = InitSem(shmClient);

    pid_t pid = fork();

    if (pid == -1)
        ReportError("Fork error.");

    if (pid != 0)
    {
        syslog(LOG_INFO, "Host run client.");

        Wolf &wolf = Wolf::GetInstance();

        bool isGameOver = false;

        while (!isGameOver)
        {
            WaitSemTimed(semHost);

            isGameOver = wolf.CatchLittleGoat();

            PostSem(semClient);
        }

        DestroySem(semHost);
        DestroySem(semClient);

        UnlinkShm(SHM_HOST);
        UnlinkShm(SHM_CLIENT);

        kill(pid, SIGTERM);
        syslog(LOG_INFO, "Client completed.");
    }
    else
    {
        sleep(1);

        LittleGoat &littleGoat = LittleGoat::GetInstance();

        while (true)
        {
            littleGoat.MakeMove();

            PostSem(semHost);
            WaitSem(semClient);
            
            littleGoat.FindOutStatus();
        }
    }
}

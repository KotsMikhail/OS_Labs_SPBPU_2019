#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

#include "conn.h"

using namespace std;

const string SHM_NAME_1 = "shm1";
const string SHM_NAME_2 = "shm2";

const int ALIVE_MAX = 100;
const int DEAD_MAX = 50;

const int ALIVE_MAX_DIFF = 70;
const int DEAD_MAX_DIFF = 20;

const int DEAD_MOVE_MAX = 2;

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

    int shm1 = OpenShm(SHM_NAME_1), shm2 = OpenShm(SHM_NAME_2);
    sem_t *sem1 = InitSem(shm1), *sem2 = InitSem(shm2);

    pid_t pid = fork();

    if (pid == -1)
        ReportError("Fork error.");

    if (pid != 0)
    {
        syslog(LOG_INFO, "Host run client.");

        Conn conn(pid, true);

        WaitSemTimed(sem2);

        printf("Game on\n");

        int cntMove = 1, cntDeadMove = 0, isAlive = 1;

        while (true)
        {
            printf("Move: %d\n", cntMove);

            int wolfNum;
            printf("Wolf: ");
            scanf("%d", &wolfNum);

            PostSem(sem1);
            WaitSemTimed(sem2);

            int littleGoatNum = conn.Read();
            int diff = abs(wolfNum - littleGoatNum);

            if (isAlive)
            {
                printf("Alive little goat: %d\n", littleGoatNum);

                if (diff <= ALIVE_MAX_DIFF)
                    printf("Diff = |%d - %d| = %d <= %d => Alive\n", wolfNum, littleGoatNum, diff, ALIVE_MAX_DIFF);
                else
                {
                    isAlive = 0;
                    printf("Diff = |%d - %d| = %d  > %d => Dead\n", wolfNum, littleGoatNum, diff, ALIVE_MAX_DIFF);
                }
            }
            else
            {
                printf("Dead little goat: %d\n", littleGoatNum);

                if (diff <= DEAD_MAX_DIFF)
                {
                    printf("Diff = |%d - %d| = %d <= %d => Alive\n", wolfNum, littleGoatNum, diff, DEAD_MAX_DIFF);
                    isAlive = 1;
                    cntDeadMove = 0;
                }
                else
                {
                    printf("Diff = |%d - %d| = %d > %d => Dead\n", wolfNum, littleGoatNum, diff, DEAD_MAX_DIFF);
                    cntDeadMove++;

                    if (cntDeadMove == 2)
                    {
                        printf("2 dead little goat's moves in a row => Game over\n");

                        DestroySem(sem1);
                        DestroySem(sem2);

                        UnlinkShm(SHM_NAME_1);
                        UnlinkShm(SHM_NAME_2);

                        kill(pid, SIGTERM);
                        syslog(LOG_INFO, "Client completed.");

                        break;
                    }
                }
            }

            conn.Write(isAlive);

            cntMove++;

            PostSem(sem1);
            WaitSemTimed(sem2);
        }
    }
    else
    {
        sleep(1);

        Conn conn(getppid(), false);

        PostSem(sem2);

        srand(time(nullptr));

        int isAlive = 1;

        while (true)
        {
            WaitSem(sem1);

            int littleGoatNum;

            if (isAlive)
                littleGoatNum = rand() % ALIVE_MAX + 1;
            else
                littleGoatNum = rand() % DEAD_MAX + 1;

            conn.Write(littleGoatNum);

            PostSem(sem2);
            WaitSemTimed(sem1);
            
            isAlive = conn.Read();

            PostSem(sem2);
        }
    }
}

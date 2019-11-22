#include "goat.h"

#include <ctime>
#include <cerrno>
#include <iostream>
#include "../utils/utils.h"
#include <cstring>
#include <csignal>
#include <unistd.h>

void Goat::Start()
{
    struct timespec ts;
    Message msg;
    while (true)
    {
#ifndef client_fifo
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += TIMEOUT;
        if (sem_timedwait(semaphore, &ts) == -1)
        {
            std::cout << "Timeout: terminating..." << std::endl;
            Terminate(errno);
        }
#endif
        if (connection.Read(&msg, sizeof(Message)))
        {
            if (CheckSelfMessage(msg))
            {
                continue;
            }
            std::cout << "--------------------------------" << std::endl;
            std::cout << "Status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
            std::cout << "Wolf number: " << msg.number << std::endl;
            if (msg.status == ALIVE)
            {
                msg.number = GetRand(RAND_LIMIT_ALIVE);
            }
            else
            {
                msg.number = GetRand(RAND_LIMIT_DEAD);
            }
            std::cout << "Goat number: " << msg.number << std::endl;
            msg.owner = GOAT;
            connection.Write(&msg, sizeof(msg));
        }
#ifndef client_fifo
        sem_post(semaphore);
#endif
    }
}

bool Goat::OpenConnection()
{
    bool res = false;
    if (connection.Open(host_pid, false))
    {
        semaphore = sem_open(SEMAPHORE_NAME, 0);
        if (semaphore == SEM_FAILED)
        {
            std::cout << "ERROR: sem_open failed with error: " << strerror(errno) << std::endl;
        }
        else
        {
            res = true;
            std::cout << "pid of created client is: " << getpid() << std::endl;
            kill(host_pid, SIGUSR1);
        }
    }
    return res;
}

Goat& Goat::GetInstance(int host_pid)
{
    static Goat instance = Goat(host_pid);
    return instance;
}

void Goat::Terminate(int signum)
{
    kill(host_pid, SIGUSR2);
    std::cout << "Goat::Terminate()" << std::endl;
    if (connection.Close() && sem_post(semaphore) == 0 && sem_close(semaphore) == 0)
    {
        exit(signum);
    }
    std::cout << "ERROR: " << strerror(errno) << std::endl;
    exit(errno);
}

bool Goat::CheckSelfMessage(Message &msg)
{
    static int count_skipped_msgs = 0;
    static struct timespec skip_start;
    bool res = false;
    if (msg.owner == GOAT)
    {
        res = true;
        count_skipped_msgs++;
        if (count_skipped_msgs == 1)
        {
            clock_gettime(CLOCK_REALTIME, &skip_start);
        }
        else
        {
            struct timespec cur_time;
            clock_gettime(CLOCK_REALTIME, &cur_time);
            if (cur_time.tv_sec - skip_start.tv_sec >= TIMEOUT)
            {
                std::cout << "Goat::CheckSelfMessage: Timeout. Terminating..." << std::endl;
                Terminate(SIGTERM);
            }
        }
#ifdef client_mq
        connection.Write(&msg, sizeof(msg));
#endif
#ifndef client_fifo
        sem_post(semaphore);
#endif
    }
    else
    {
        count_skipped_msgs = 0;
    }
    return res;
}

Goat::Goat(int pid)
{
    std::cout << "host pid: " << pid << std::endl;
    host_pid = pid;
    signal(SIGTERM, SignalHandler);
}

void Goat::SignalHandler(int signum)
{
    //host_pid is random number because id does not matter, function is static
    Goat& instance = Goat::GetInstance(13);
    instance.Terminate(signum);
}


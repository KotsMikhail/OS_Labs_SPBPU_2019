#include "wolf.h"

#include <iostream>
#include <cstring>
#include <csignal>
#include <ctime>
#include <fcntl.h>
#include "../utils/utils.h"
#include <unistd.h>
#include <cerrno>


void Wolf::Start()
{
    struct timespec ts;
    Message msg;

    while (true)
    {
        if (!client_info.attached)
        {
            std::cout << "Waiting for client..." << std::endl;
#ifndef host_fifo
            sem_wait(semaphore);
#endif
            pause();
            std::cout << "Client attached!" << std::endl;
            curr_num = GetRand(RAND_LIMIT_WOLF);
            std::cout << "Wolf current number: " << curr_num << std::endl;
            msg = Message(WOLF, ALIVE, curr_num);
            connection.Write(&msg, sizeof(msg));
#ifndef host_fifo
            sem_post(semaphore);
#endif
        }
        else
        {
#ifndef host_fifo
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            if (sem_timedwait(semaphore, &ts) == -1)
            {
                kill(client_info.pid, SIGTERM);
                client_info = ClientInfo(0);
                continue;
            }
#endif
            if (connection.Read(&msg, sizeof(Message)))
            {
                if (CheckSelfMessage(msg))
                {
                    continue;
                }
                std::cout << "--------------------------------" << std::endl;
                std::cout << "Goat current status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
                std::cout << "Wolf number: " << curr_num << std::endl;
                std::cout << "Goat number: " << msg.number << std::endl;
                msg = Step(msg);
                if (msg.number == 0) {
                    std::cout << "Game is over!" << std::endl;
                    continue;
                }
                else {
                    std::cout << "Goat new status: " << ((msg.status == ALIVE) ? "alive" : "dead") << std::endl;
                }
                connection.Write(&msg, sizeof(msg));
            }
#ifndef host_fifo
            sem_post(semaphore);
#endif
        }
    }
}

bool Wolf::OpenConnection()
{
    bool res = false;
    int pid = getpid();
    if (connection.Open(pid, true))
    {
        semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
        if (semaphore == SEM_FAILED)
        {
            std::cout << "ERROR: sem_open failed with error = " << strerror(errno) << std::endl;
        }
        else
        {
            res = true;
            std::cout << "pid of created host is: " << pid << std::endl;
        }
    }
    return res;
}

Wolf& Wolf::GetInstance()
{
    static Wolf instance;
    return instance;
}

void Wolf::Terminate(int signum)
{
    std::cout << "Wolf::Terminate()" << std::endl;
    if (connection.Close() && sem_unlink(SEMAPHORE_NAME) == 0)
    {
        exit(signum);
    }
    exit(errno);
}

bool Wolf::CheckSelfMessage(Message &msg)
{
    static int count_skipped_msgs = 0;
    static struct timespec skip_start;
    bool res = false;
    if (msg.owner == WOLF)
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
                std::cout << "Timeout: terminating client.." << std::endl;
                kill(client_info.pid, SIGTERM);
                client_info = ClientInfo(0);
            }
        }
#ifdef host_mq
        connection.Write(&msg, sizeof(msg));
#endif
#ifndef host_fifo
        sem_post(semaphore);
#endif
    }
    else
    {
        count_skipped_msgs = 0;
    }
    return res;
}

Wolf::Wolf() : client_info(0), curr_num(0)
{
    struct sigaction act;
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGUSR2, &act, nullptr);
}

Message Wolf::Step(Message &ans)
{
    Message msg;
    if ((ans.status == ALIVE && abs(curr_num - ans.number) <= 70) ||
        (ans.status == DEAD && abs(curr_num - ans.number) <= 20))
    {
        client_info.count_dead = 0;
    }
    else
    {
        msg.status = DEAD;
        client_info.count_dead++;
        if (client_info.count_dead == 2)
        {
            kill(client_info.pid, SIGTERM);
            client_info = ClientInfo(0);
            msg.status = ALIVE;
            return msg;
        }
    }
    curr_num = GetRand(RAND_LIMIT_WOLF);
    msg.number = curr_num;
    return msg;
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr)
{
    static Wolf& instance = GetInstance();
    switch (signum)
    {
        case SIGUSR1:
        {
            if (instance.client_info.attached)
            {
                std::cout << "Only one client could be handled" << std::endl;
            }
            else
            {
                std::cout << "Attaching client with pid: " << info->si_pid << std::endl;
                instance.client_info = ClientInfo(info->si_pid);
            }
            break;
        }
        case SIGUSR2: // client terminated
        {
            std::cout << "Client terminated" << std::endl;
            if (instance.client_info.pid == info->si_pid)
            {
                instance.client_info = ClientInfo(0);
            }
            break;
        }
        default:
        {
            if (instance.client_info.attached)
            {
                kill(instance.client_info.pid, SIGTERM);
                instance.client_info = ClientInfo(0);
            }
            instance.Terminate(signum);
        }
    }
}

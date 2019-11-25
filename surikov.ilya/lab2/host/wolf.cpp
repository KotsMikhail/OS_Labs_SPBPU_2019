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
            sem_wait(semaphore_client);
            while (!client_info.attached)
            {
                pause();
            }
            std::cout << "Client attached!" << std::endl;
            do {
                std::cout << "Enter the number from 1 to 100" << std::endl;
                std::cin >> curr_num;
            } while (curr_num < 1 || curr_num > RAND_LIMIT);
            std::cout << "Wolf current number: " << curr_num << std::endl;
            msg = Message(Status::ALIVE, curr_num);
            connection.Write(&msg, sizeof(msg));
            sem_post(semaphore_client);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            if (sem_timedwait(semaphore_host, &ts) == -1)
            {
                kill(client_info.pid, SIGTERM);
                client_info = ClientInfo(0);
                continue;
            }
            if (connection.Read(&msg, sizeof(Message)))
            {
                std::cout << "--------------------------------" << std::endl;
                std::cout << "Goat current status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;
                std::cout << "Wolf number: " << curr_num << std::endl;
                std::cout << "Goat number: " << msg.number << std::endl;
                msg = Step(msg);
                if (msg.number == 0) {
                    std::cout << "Game is over!" << std::endl;
                    continue;
                }
                else {
                    std::cout << "Goat new status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;
                }
                connection.Write(&msg, sizeof(msg));
            }
            sem_post(semaphore_client);
        }
    }
}

bool Wolf::OpenConnection()
{
    bool res = false;
    int pid = getpid();
    if (connection.Open(pid, true))
    {
        semaphore_host = sem_open(SEMAPHORE_HOST_NAME, O_CREAT, 0666, 0);
        semaphore_client = sem_open(SEMAPHORE_CLIENT_NAME, O_CREAT, 0666, 0);
        if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED)
        {
            std::cout << "ERROR: sem_open failed with error: " << strerror(errno) << std::endl;
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
    if (sem_unlink(SEMAPHORE_HOST_NAME) == -1 || sem_unlink(SEMAPHORE_CLIENT_NAME) == -1)
    {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    if (!connection.Close())
    {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    exit(signum);
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
    if ((ans.status == Status::ALIVE && abs(curr_num - ans.number) <= 70) ||
        (ans.status == Status::DEAD && abs(curr_num - ans.number) <= 20))
    {
        client_info.count_dead = 0;
    }
    else
    {
        msg.status = Status::DEAD;
        client_info.count_dead++;
        if (client_info.count_dead == 2)
        {
            kill(client_info.pid, SIGTERM);
            client_info = ClientInfo(0);
            msg.status = Status::ALIVE;
            return msg;
        }
    }
    do {
        std::cout << "Enter the number from 1 to 100" << std::endl;
        std::cin >> curr_num;
    } while (curr_num < 1 || curr_num > RAND_LIMIT);
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

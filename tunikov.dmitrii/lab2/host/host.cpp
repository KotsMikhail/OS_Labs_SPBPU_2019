//
// Created by dmitrii on 19.11.2019.
//
#include <iostream>
#include <cstring>
#include <csignal>
#include <zconf.h>
#include <fcntl.h>
#include "host.h"

int main()
{
    std::cout << "Host starting.." << std::endl;

    Host& h = Host::getInstance();
    if (h.openConnection())
    {
        h.run();
    }
    return 0;
}

bool Host::openConnection()
{
    bool res = false;

    if (connection.Open(getpid(), true))
    {
        semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
        if (semaphore == SEM_FAILED)
        {
            std::cout << "ERROR: can't open semathore" << std::endl;
        }
        else
        {
            res = true;
            std::cout << "host created with pid: " << getpid() << std::endl;
        }
    }

    return res;
}

Host &Host::getInstance()
{
    static Host inst;
    return inst;
}

Host::Host() : client_data(0)
{
    srand(time(nullptr));
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGUSR2, &act, nullptr);
}

void Host::run()
{
#ifndef host_fifo
    struct timespec ts{5 , 0};
#endif

    std::cout << "wait for client attach.." << std::endl;
    pause();

    Date cur_date = getRandDate();

#ifndef host_fifo
    sem_wait(semaphore);
#endif
    Memory mem(cur_date, HOST);
    mem.m_date.printDate("date from HOST");
    connection.Write(&mem);
#ifndef host_fifo
    sem_post(semaphore);
#endif
    while (true)
    {
       if (!client_data.attached)
       {
           std::cout << "Waiting for client..." << std::endl;
#ifndef host_fifo
           sem_wait(semaphore);
#endif
           pause();
           mem = Memory(cur_date, HOST);
           mem.m_date.printDate("date from HOST");
           connection.Write(&mem, sizeof(mem));
#ifndef host_fifo
           sem_post(semaphore);
#endif
       }
       else
       {
#ifndef host_fifo
           clock_gettime(CLOCK_REALTIME, &ts);
           ts.tv_sec += TIMEOUT;

           //wait semathore signal
           if (sem_timedwait(semaphore, &ts) == -1)
           {
               std::cout << "ERROR: can't sem_timedwait, error is " << errno << std::endl;
               std::cout << "kill client with pid " << client_data.pid << std::endl;
               kill(client_data.pid, SIGTERM);
               client_data = ClientData(0);
               continue;
           }
#endif
           //read msg
           if (connection.Read(&mem))
           {
               if (checkTimeout(mem))
                   continue;

               //output date from client
               std::cout << "--------------------------------------------" << std::endl;
               std::cout << "receive temperature from client: " << mem.m_temperature << std::endl;

               //generate new date and send to client
               Date new_date = getRandDate();
               mem.m_owner = HOST;
               mem.m_date = new_date;
               mem.m_date.printDate("send from HOST");
               connection.Write(&mem);
#ifndef host_fifo
               //post sem
               sem_post(semaphore);
#endif
           }
           else
           {

           }
       }
    }
}

bool Host::checkTimeout(Memory& mem)
{
    bool res = false;
    static timespec start_wait;

    if (mem.m_owner == HOST)
    {
        res = true;
        client_data.skiped_messages++;
        if (client_data.skiped_messages == 1)
        {
            clock_gettime(CLOCK_REALTIME, &start_wait);
        }
        else
        {
            timespec cur_time{};
            clock_gettime(CLOCK_REALTIME, &cur_time);
            if (cur_time.tv_sec - start_wait.tv_sec >= TIMEOUT)
            {
                kill(client_data.pid, SIGTERM);
                client_data = ClientData(0);
            }
        }
#ifdef host_mq
        connection.Write(&mem);
#endif
#ifndef host_fifo
        sem_post(semaphore);
#endif
    }
    else
    {
        client_data.skiped_messages = 0;
    }

    return res;
}

void Host::signalHandler(int sig, siginfo_t* info, void* ptr)
{
    static Host&h = getInstance();
    switch (sig)
    {
        case SIGUSR1:
            if (h.client_data.pid == info->si_pid)
            {
                h.client_data = ClientData(0);
            }
            else
            {
                std::cout << "attached client with pid " << info->si_pid << std::endl;
                h.client_data = ClientData(info->si_pid);
            }
            break;
        case SIGUSR2:
            std::cout << "client fail" << std::endl;
            if (h.client_data.pid == info->si_pid)
            {
                h.client_data = ClientData(0);
            }
            break;
        case SIGTERM:
            if (h.client_data.attached)
            {
                kill(h.client_data.pid, SIGTERM);
                h.client_data = ClientData(0);
            }
            Host::close(sig);
        default:
            std::cout << "unknown signal accepted" << std::endl;
            exit(sig);
    }
}

Date Host::getRandDate()
{
    return Date((rand() % 31) + 1,
            (rand() % 12) + 1, (rand() % 2019) + 1);
}

void Host::close(int exit_code)
{
    static Host&h = Host::getInstance();

    std::cout << "host terminating..." << std::endl;

    if (h.connection.Close() && sem_unlink(SEM_NAME) == 0)
    {
        exit(exit_code);
    }
    exit(errno);
}

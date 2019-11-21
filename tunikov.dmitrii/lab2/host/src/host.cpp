//
// Created by dmitrii on 19.11.2019.
//
#include <iostream>
#include <csignal>
#include <zconf.h>
#include <cstring>
#include <bits/fcntl-linux.h>
#include "host.h"
#include "../../interfaces/memory.h"


ClientData Host::client_data(0);

int main()
{
    std::cout << "Host starting.." << std::endl;

    Host& h = Host::getInstance();
    h.run();

    srand(time(nullptr));

    return 0;
}


Host &Host::getInstance()
{
    static Host inst;
    return inst;
}

Host::Host() : connection(getpid(), true)
{
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED)
    {
        std::cout << "ERROR: cant' sem_open, error = " << errno << std::endl;
    }
    else
    {
        std::cout << "created host with pid: " << getpid() << std::endl;
    }
}

Host::~Host()
{

}

void Host::run()
{
    struct timespec ts{5 , 0};

    std::cout << "wait for client attach.." << std::endl;
    pause();

    std::string cur_date = getRandDate();

    sem_wait(semaphore);
    Memory mess(cur_date);
    connection.Write(&mess, sizeof(mess));
    sem_post(semaphore);

    while (true)
    {
       if (!client_data.attached)
       {
           std::cout << "wait for client.." << std::endl;
           pause();
       }
       else
       {
           if (sem_timedwait(semaphore, &ts) == -1)
           {
               std::cout << "ERROR: can't sem_timedwait, error is " << errno << std::endl;
               std::cout << "kill client with pid " << client_data.pid << std::endl;
               kill(client_data.pid, SIGTERM);
               client_data = ClientData(0);
               continue;
           }
       }

       break;
    }
}

void Host::signalHandler(int sig, siginfo_t* info, void* ptr)
{
    switch (sig)
    {
        case SIGUSR1:
            if (client_data.pid == info->si_pid)
            {
                client_data = ClientData(0);
            }
            else
            {
                std::cout << "attached client with pid " << info->si_pid << std::endl;
                client_data = ClientData(info->si_pid);
            }
            break;
        case SIGTERM:
            exit(0);
        default:
            std::cout << "unknown signal accepted" << std::endl;
            exit(sig);
    }
}

std::string Host::getRandDate()
{
    int randomYear = (rand() % 2019) + 1;
    int randomMonth = (rand() % 12) + 1;
    int randomDays = (rand() % 30) + 1;

    std::string date = std::to_string(randomDays) + "."
            + std::to_string(randomMonth) + "." + std::to_string(randomYear);

    return date;
}

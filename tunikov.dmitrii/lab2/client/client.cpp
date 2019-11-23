//
// Created by dmitrii on 23.11.2019.
//

#include "client.h"

#include <iostream>
#include <zconf.h>
#include <csignal>
#include <cstring>
#include <semaphore.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Run by: ./client_<connection_type> <host pid>" << std::endl;
        return 1;
    }
    int pid;
    try
    {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR: can't convert to int pid argument" << std::endl;
        return -1;
    }
    Client& client = Client::getInstance(pid);
    if (client.openConnection())
    {
        client.run();
    }
    return 0;
}

Client& Client::getInstance(int host_pid)
{
    static Client inst = Client(host_pid);
    return inst;
}

Client::Client(int host_pid) : skiped_messages(0)
{
    std::cout << "host Wolf pid is: " << host_pid << std::endl;
    this->host_pid = host_pid;
    need_post = false;
    signal(SIGTERM, signalHandler);
}

bool Client::openConnection()
{
    bool res = false;
    if (connection.Open(host_pid, false))
    {
        semaphore = sem_open(SEM_NAME, 0);
        if (semaphore == SEM_FAILED)
        {
            std::cout << "ERROR: sem_open failed with error = " << strerror(errno) << std::endl;
        }
        else
        {
            res = true;
            std::cout << "pid of created Client is: " << getpid() << std::endl;
            kill(host_pid, SIGUSR1);
        }
    }
    return res;
}

void Client::close(int signum)
{
    kill(host_pid, SIGUSR2);
    std::cout << "client terminating.." << std::endl;
    if (errno != 0)
    {
        std::cout << "Failing, ERROR = " << strerror(errno) << std::endl;
    }
    if (connection.Close())
    {
        if (need_post && sem_post(semaphore) == -1)
        {
            exit(errno);
        }
        if (sem_close(semaphore) == -1)
        {
            exit(errno);
        }
        exit(signum);
    }
    std::cout << "ERROR while terminating: " << strerror(errno) << std::endl;
    exit(errno);
}

void Client::run()
{
    while (true)
    {
#ifndef client_fifo
        struct timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += TIMEOUT;
        if (sem_timedwait(semaphore, &ts) == -1)
        {
            close(errno);
        }
        need_post = true;
#endif
        Memory mem;
        if (connection.Read(&mem))
        {
            if (checkTimeout(mem))
            {
                continue;
            }
            std::cout << "---------------------------------------" << std::endl;
            mem.m_date.printDate("client read date");
            int temp = getRandTemp(mem.m_date);
            std::cout << "client send message: " << temp << std::endl;
            mem.m_owner = CLIENT;
            mem.m_temperature = temp;
            connection.Write((void *)&mem, sizeof(mem));
        }
#ifndef client_fifo
        sem_post(semaphore);
        need_post = false;
#endif
    }
}

int Client::getRandTemp(const Date& date)
{
    srand(std::hash<std::string>{}(std::to_string(date.m_day) + std::to_string(date.m_day) + std::to_string(date.m_day)));
    return rand();
}

void Client::signalHandler(int signum)
{
    Client& client = Client::getInstance(30);
    client.close(signum);
}

bool Client::checkTimeout(Memory &mem) {
    static struct timespec skip_start;
    bool res = false;
    if (mem.m_owner == CLIENT)
    {
        res = true;
        skiped_messages++;
        if (skiped_messages == 1)
        {
            clock_gettime(CLOCK_REALTIME, &skip_start);
        }
        else
        {
            struct timespec cur_time{};
            clock_gettime(CLOCK_REALTIME, &cur_time);
            if (cur_time.tv_sec - skip_start.tv_sec >= TIMEOUT)
            {
                close(SIGTERM);
            }
        }
#ifdef client_mq
        connection.Write(&mem, sizeof(mem));
#endif
#ifndef client_fifo
        sem_post(semaphore);
        need_post = false;
#endif
    }
    else
    {
        skiped_messages = 0;
    }
    return res;
}

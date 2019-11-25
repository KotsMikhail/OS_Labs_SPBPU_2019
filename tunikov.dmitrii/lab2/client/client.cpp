//
// Created by dmitrii on 23.11.2019.
//

#include "client.h"
#include "constants.h"
#include <iostream>
#include <zconf.h>
#include <csignal>
#include <cstring>
#include <semaphore.h>
#include <random>
#include <fcntl.h>

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

Client::Client(int host_pid_)
{
    host_pid = host_pid_;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
}

bool Client::openConnection()
{
    std::cout << "opening connection for client with pid: " << getpid() << std::endl;

    client_semaphore = sem_open(constants::client_sem_name, O_CREAT, 0666, 1);
    if (client_semaphore == SEM_FAILED)
    {
        std::cout << "ERROR: can't open client semathore" << std::endl;
        return false;
    }
    host_semaphore = sem_open(constants::host_sem_name, O_CREAT, 0666, 1);
    if (host_semaphore == SEM_FAILED)
    {
        std::cout << "ERROR: can't open host semathore" << std::endl;
        sem_unlink(constants::client_sem_name);
        return false;
    }

    connection = Conn();
    if (!connection.Open(getpid(), false))
    {
        std::cout << "ERROR: fail open connection" << std::endl;
        sem_unlink(constants::client_sem_name);
        sem_unlink(constants::host_sem_name);
        return false;
    }

    kill(host_pid, SIGUSR1);
    std::cout << "client created with pid " << getpid() << std::endl;
    return true;
}

void Client::close(int signum)
{
    kill(host_pid, SIGUSR2);
    std::cout << "client terminating.." << std::endl;
    if (host_semaphore != SEM_FAILED)
    {
        if (sem_close(host_semaphore) == -1)
        {
            std::cout << "ERROR: fail to close host sem" << std::endl;
            exit(errno);
        }
    }
    if (client_semaphore != SEM_FAILED)
    {
        if (sem_close(client_semaphore) == -1)
        {
            std::cout << "ERROR: fail to close client sem" << std::endl;
            exit(errno);
        }
    }

    if (connection.Close())
    {
        exit(signum);
    }

    std::cout << "ERROR while terminating: " << strerror(errno) << std::endl;
    exit(errno);
}

void Client::run()
{
    Message msg;
    while (true)
    {
        sem_wait(client_semaphore);
        if (connection.Read(&msg))
        {
            std::cout << "client read date: " << msg.m_date.m_day << "." << msg.m_date.m_month << "." << msg.m_date.m_year << std::endl;
            msg.m_temperature = getRandTemp(msg.m_date);
            std::cout << "temp value: " << msg.m_temperature << std::endl;
            connection.Write(&msg);
        }
        sem_post(host_semaphore);
    }
}

int Client::getRandTemp(const Date& date)
{
    unsigned long str_date = std::stoul(std::to_string(date.m_day) + std::to_string(date.m_month) + std::to_string(date.m_year));
    std::minstd_rand mt(str_date);
    std::uniform_int_distribution<int> temp(-100, 100);
    return temp(mt);
}

void Client::signalHandler(int sig, siginfo_t *info, void *context)
{
    Client& client = Client::getInstance(0);
    client.close(sig);
}
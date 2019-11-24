//
// Created by dmitrii on 19.11.2019.
//
#include <iostream>
#include <cstring>
#include <csignal>
#include <zconf.h>
#include <fcntl.h>
#include <sstream>
#include <vector>
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
    std::cout << "opening connection for host with pid: " << getpid() << std::endl;

    client_semaphore = sem_open(CLIENT_SEM_NAME, O_CREAT, 0666, 1);
    if (client_semaphore == SEM_FAILED)
    {
        std::cout << "ERROR: can't open client semathore" << std::endl;
        return false;
    }
    host_semaphore = sem_open(HOST_SEM_NAME, O_CREAT, 0666, 1);
    if (host_semaphore == SEM_FAILED)
    {
        std::cout << "ERROR: can't open host semathore" << std::endl;
        sem_unlink(CLIENT_SEM_NAME);
        return false;
    }

    if (!connection.Open(getpid(), true))
    {
        std::cout << "ERROR: fail open connection" << std::endl;
        sem_unlink(CLIENT_SEM_NAME);
        sem_unlink(HOST_SEM_NAME);
        return false;
    }

    std::cout << "host created with pid " << getpid() << std::endl;
    return true;
}

Host &Host::getInstance()
{
    static Host inst;
    return inst;
}

Host::Host() : client_data(0)
{
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGUSR2, &act, nullptr);
}

bool Host::readDate(Message &msg)
{
    std::string str_date, cur_sym;
    std::cin.clear();
    std::cin >> str_date;

    if (str_date.length() == 0)
        return false;

    std::vector<int> date_vec;
    std::istringstream ss(str_date);
    while (std::getline(ss, cur_sym, '.'))
    {
        try
        {
            unsigned t = std::stoul(cur_sym);
            date_vec.push_back(t);
        }
        catch (const std::exception& e)
        {
            std::cout << "ERROR: cant' parse date" << std::endl;
            return false;
        }
    }

    if (!client_data.attached)
        return false;

    if (date_vec.size() != 3 || (date_vec[0] < 0 || date_vec[0] > 31) || (date_vec[1] < 0 || date_vec[1] > 12))
    {
        std::cout << "ERROR: wrong date format" << std::endl;
        return false;
    }

    msg.m_date = {date_vec[0], date_vec[1], date_vec[2]};

    return true;
}

void Host::run()
{
    std::cout << "input date to count temperature" << std::endl;

    struct timespec ts{5 , 0};
    Message msg;
    int ec;

    while (true)
    {
        if (!client_data.attached)
            sleep(1);
        else
        {
            if (!readDate(msg))
                continue;

            if (!client_data.attached)
                continue;

            //write message
#ifndef host_sock
            connection.Write(&msg);
#else
            if (!connection.Open(getpid(), true))
            {
                close();
            }
            connection.Write(&msg);
#endif

            //post client semathore
            sem_post(client_semaphore);

            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            //wait for client post host semathore
            while ((ec = sem_timedwait(host_semaphore, &ts)) == -1 && errno == EINTR)
                continue;

            if (ec == -1)
            {
                if (client_data.attached)
                {
                    kill(client_data.pid, SIGTERM);
                    std::cout << "kill client with pid: " << client_data.pid << std::endl;
                }
                client_data = ClientData(0);
            }
            else if (connection.Read(&msg))
            {
                std::cout << "client send temperature: " << msg.m_temperature << std::endl;
            }
        }
    }
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
            std::cout << "client terminating" << std::endl;
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

void Host::close(int exit_code)
{
    static Host&h = Host::getInstance();

    std::cout << "host terminating..." << std::endl;

    if (h.connection.Close() && sem_unlink(CLIENT_SEM_NAME) == 0 && sem_unlink(HOST_SEM_NAME))
    {
        exit(exit_code);
    }
    exit(errno);
}

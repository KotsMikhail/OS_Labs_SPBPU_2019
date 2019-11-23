#include <iostream>
#include <csignal>
#include <random>
#include <cstring>
#include <unistd.h>
#include <message.h>
#include <fcntl.h>

#include "client.h"


static void ClientSignalHandler (int signalNum)
{
    Client& inst = Client::GetInstance(0);
    inst.Terminate();
}


Client& Client::GetInstance (pid_t pid)
{
    static Client inst = Client(pid);
    return inst;
}


Client::Client (pid_t pid)
: hostPid(pid)
{
    std::cout << "Client::Client(). Host pid: " << pid << std::endl;
    signal(SIGTERM, ClientSignalHandler);
}


bool Client::OpenConnection ()
{
    if ((semaphore_host = sem_open(HOST_SEM_NAME, O_CREAT)) == SEM_FAILED) {
        std::cout << "[ERROR]: failed to create host semaphore, error: " << strerror(errno) << std::endl;
        return false;
    }

    if ((semaphore_client = sem_open(CLIENT_SEM_NAME, O_CREAT)) == SEM_FAILED) {
        std::cout << "[ERROR]: failed to create client semaphore, error: " << strerror(errno) << std::endl;
        sem_unlink(HOST_SEM_NAME);
        return false;
    }

    std::cout << "Semaphores created. Host pid: " << getpid() << std::endl;
    if (!conn.Open(getpid(), false)) {
        std::cout << "[ERROR]: Failed to open connection." << std::endl;
        sem_unlink(HOST_SEM_NAME);
        sem_unlink(CLIENT_SEM_NAME);
        return false;
    }

    kill(hostPid, SIGUSR1);
    return true;
}


void Client::Terminate ()
{
    std::cout << "Terminating client..." << std::endl;
    kill(hostPid, SIGUSR2);
    if (semaphore_host != SEM_FAILED && sem_close(semaphore_host) == -1) {
        std::cout << "[ERROR]: Failed to close host semaphore, error: " << strerror(errno) << std::endl;
        exit(errno);
    }

    if (semaphore_client != SEM_FAILED && sem_close(semaphore_client) == -1) {
        std::cout << "[ERROR]: Failed to close client semaphore, error: " << strerror(errno) << std::endl;
        exit(errno);
    }

    if (conn.Close()) {
        exit(SIGTERM);
    }

    std::cout << "[ERROR]: Failed in client terminate, error: " << strerror(errno) << std::endl;
    exit(errno);
}


int Client::GetNewRandomNumber (int rightRandEdge)
{
    std::random_device randDev;
    std::mt19937 mt(randDev());
    std::uniform_int_distribution<int> uniformDist(MIN_RAND, rightRandEdge);
    return uniformDist(mt);
}


void Client::Start ()
{
    Message msg;
    while (true) {
        sem_wait(semaphore_client);

        if (conn.Read(&msg)) {
            std::cout << "------------CLIENT-----------" << std::endl;
            std::cout << "Status: " << ((msg.status == MSG_STATUS::ALIVE) ? "ALIVE" : "DEAD") << std::endl;

            if (msg.status == MSG_STATUS::ALIVE) {
                msg.number = GetNewRandomNumber(ALIVE_MAX_RAND);
            } else {
                msg.number = GetNewRandomNumber(DEAD_MAX_RAND);
            }

            std::cout << "Client new number: " << msg.number << std::endl;
            std::cout << "------------CLIENT-----------" << std::endl;

            msg.owner = MSG_OWNER::CLIENT;
            conn.Write((void *)&msg);
        }

        sem_post(semaphore_host);
    }
}

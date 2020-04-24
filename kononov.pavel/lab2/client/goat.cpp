#include "goat.h"

#include <cerrno>
#include <iostream>
#include "../utils/utils.h"
#include <cstring>
#include <unistd.h>

int Goat::id;

void Goat::Start() {
    Message msg;

    msg.number = GetRand(RAND_LIMIT_ALIVE);
    std::cout << "Started goat number:" << msg.number << std::endl;
    sem_wait(semaphore_client);
    connection.Write(&msg, sizeof(msg));
    sem_post(semaphore_host);
    while (true) {
        sem_wait(semaphore_client);
        if (connection.Read(&msg, sizeof(Message))) {
            std::cout << "--------------------------------" << std::endl;
            std::cout << "Status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << " id: " << id << std::endl;
            std::cout << "Wolf number: " << msg.number << std::endl;
            if (msg.status == Status::ALIVE) {
                msg.number = GetRand(RAND_LIMIT_ALIVE);
            } else {
                msg.number = GetRand(RAND_LIMIT_DEAD);
            }
            std::cout << "Goat number: " << msg.number << std::endl;
            connection.Write(&msg, sizeof(msg));
        }
        sem_post(semaphore_host);
    }
}

bool Goat::OpenConnection() {
    bool res = false;

    alarm(5);
    kill(host_pid, SIGUSR1);
    while (id < 0)
        pause();

    if (connection.Open(id, false)) {
        semaphore_host = sem_open(GetName(SEMAPHORE_HOST_NAME, id).c_str(), 0);
        semaphore_client = sem_open(GetName(SEMAPHORE_CLIENT_NAME, id).c_str(), 0);
        if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED) {
            std::cout << "ERROR: sem_open failed with error: " << strerror(errno) << std::endl;
        } else {
            res = true;
            std::cout << "pid of created client is: " << getpid() << std::endl;
        }
    }
    return res;
}

Goat& Goat::GetInstance(int host_pid) {
    static Goat instance(host_pid);
    return instance;
}

void Goat::Terminate(int signum) {
    kill(host_pid, SIGUSR2);
    std::cout << "Exit" << std::endl;
    if (sem_close(semaphore_client) == -1 || sem_close(semaphore_host) == -1) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    if (!connection.Close()) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    exit(signum);
}

Goat::Goat(int pid) {
    std::cout << "host pid: " << pid << std::endl;
    host_pid = pid;
    id = -1;
    struct sigaction act;
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGALRM, &act, nullptr);
}

void Goat::SignalHandler(int signum, siginfo_t* info, void* ptr) {
    switch (signum) {
        case SIGUSR1: {
            id = info->si_value.sival_int;
            break;
        }
        case SIGALRM: {
            if (id >= 0)
                break;
        }
        default: {
            Goat &instance = Goat::GetInstance(13);
            instance.Terminate(signum);
        }
    }
}


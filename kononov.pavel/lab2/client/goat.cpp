#include "goat.h"

#include <cerrno>
#include <iostream>
#include "../utils/utils.h"
#include <cstring>
#include <unistd.h>


void Goat::Start() {
    Message msg;

    msg.number = GetRand(RAND_LIMIT_ALIVE);
    std::cout << "Goat number:" << msg.number << std::endl;
    m_connection.Write(&msg, sizeof(msg));
    sem_post(m_semaphore_host);
    while (true) {
        sem_wait(m_semaphore_client);
        if (m_connection.Read(&msg, sizeof(Message))) {
            std::cout << "Wolf number: " << msg.number << std::endl;
            std::cout << "Status: " << ((msg.status == Status::ALIVE) ? "alive" : "dead") << " id: " << m_id
                      << std::endl;
            std::cout << "--------------------------------" << std::endl;
            if (msg.status == Status::ALIVE) {
                msg.number = GetRand(RAND_LIMIT_ALIVE);
            } else {
                msg.number = GetRand(RAND_LIMIT_DEAD);
            }
            std::cout << "Goat number: " << msg.number << std::endl;
            m_connection.Write(&msg, sizeof(msg));
        }
        sem_post(m_semaphore_host);
    }
}

bool Goat::OpenConnection() {
    bool res = false;

    if (m_host_pid == 0)
        return false;

    alarm(5);
    kill(m_host_pid, SIGUSR1);
    while (m_id < 0)
        pause();

    if (m_connection.Open(m_id, false)) {
        m_semaphore_host = sem_open(GetName(SEMAPHORE_HOST_NAME, m_id).c_str(), 0);
        m_semaphore_client = sem_open(GetName(SEMAPHORE_CLIENT_NAME, m_id).c_str(), 0);
        if (m_semaphore_host == SEM_FAILED || m_semaphore_client == SEM_FAILED) {
            std::cout << "ERROR: sem_open failed with error: " << strerror(errno) << std::endl;
        } else {
            res = true;
            std::cout << "pid of created client is: " << getpid() << std::endl;
        }
    }
    return res;
}

void Goat::SetHostPid(int pid) {
    std::cout << "host pid: " << pid << std::endl;
    m_host_pid = pid;
}

Goat& Goat::GetInstance() {
    static Goat instance;
    return instance;
}

void Goat::Terminate(int signum) {
    kill(m_host_pid, SIGUSR2);
    std::cout << "Exit" << std::endl;
    if (sem_close(m_semaphore_client) == -1 || sem_close(m_semaphore_host) == -1) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    if (!m_connection.Close()) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    exit(signum);
}

Goat::Goat() {
    m_host_pid = 0;
    m_id = -1;
    struct sigaction act;
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGALRM, &act, nullptr);
}

void Goat::SignalHandler(int signum, siginfo_t* info, void* ptr) {
    Goat &instance = Goat::GetInstance();
    switch (signum) {
        case SIGUSR1: {
            instance.m_id = info->si_value.sival_int;
            break;
        }
        case SIGALRM: {
            if (instance.m_id >= 0)
                break;
        }
        default: {
            instance.Terminate(signum);
        }
    }
}


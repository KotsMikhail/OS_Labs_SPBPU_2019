#include "wolf.h"

#include <iostream>
#include <limits>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <fcntl.h>
#include "../utils/utils.h"

void Wolf::Start() {
    int attached_clients = 0;

    if (m_clientsNum == 0)
        return;

    while (attached_clients != m_clientsNum) {
        attached_clients = 0;
        for (int i = 0; i < m_clientsNum; ++i) {
            if (m_client_info[i].attached)
                attached_clients++;
        }

        if (attached_clients != m_clientsNum) {
            std::cout << "Waiting " << m_clientsNum - attached_clients << " clients." << std::endl;
            pause();
        }
    }
    std::cout << "Clients attached!" << std::endl;

    for (int i = 0; i < m_clientsNum; ++i) {
        pthread_attr_init(&m_attr[i]);
        pthread_create(&(m_threads[i]), &m_attr[i], ThreadRun, &m_client_info[i]);
    }

    while (true) {
        if (m_finished == m_clientsNum) {
            pthread_mutex_lock(&m_mutx);
            int res = 0;
            for (int i = 0; i < m_clientsNum; ++i) {
                if (m_client_info[i].count_dead < 2)
                    ++res;
            }

            if (res == 0) {
                std::cout << "All goats are dead during 2 steps, game over." << std::endl;
                Terminate(SIGINT);
            }

            std::cout.flush(); // ??
            GetNumber();
            std::cout.flush(); // ??
            m_finished = 0;
            m_step++;
            pthread_cond_broadcast(&m_cond);
            pthread_mutex_unlock(&m_mutx);
        } else {
            sleep(1);
        }
    }
}

bool Wolf::OpenConnection() {
    bool res = true;

    for (int i = 0; i < m_clientsNum; ++i) {
        res &= m_client_info[i].OpenConnection(i);
    }

    if (res)
        std::cout << "Created host pid: " << getpid() << std::endl;
    else
        std::cout << "Open connection error" << std::endl;

    return res;
}

Wolf& Wolf::GetInstance() {
    static Wolf instance;
    return instance;
}

void Wolf::Terminate(int signum) {
    std::cout << "Exit" << std::endl;

    for (int i = 0; i < m_clientsNum; ++i)
        m_client_info[i].Dettach();

    for (int i = 0; i < m_clientsNum; ++i)
        kill(m_client_info[i].pid, signum);

    for (int i = 0; i < m_clientsNum; i++)
        pthread_cancel(m_threads[i]);

    exit(signum);
}

void Wolf::SetClientsNum(int n) {
    m_clientsNum = n;
    delete[] m_client_info;
    delete[] m_threads;
    delete[] m_attr;
    m_client_info = new ClientInfo[n];
    m_threads = new pthread_t[n];
    m_attr = new pthread_attr_t[n];
}

Wolf::Wolf() {
    m_step = 0;
    m_curr_num = 0;
    m_finished = 0;
    m_clientsNum = 0;

    struct sigaction act;
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGUSR2, &act, nullptr);

    m_client_info = nullptr;
    m_threads = nullptr;
    m_attr = nullptr;

    pthread_cond_init(&m_cond, NULL);
    pthread_mutex_init(&m_mutx, NULL);
}

Wolf::~Wolf() {
    for (int i = 0; i < m_clientsNum; i++)
        m_client_info[i].Delete();

    delete[]m_threads;
    delete[]m_attr;
    delete[]m_client_info;

    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutx);
}

Message Wolf::Step(Message &ans, ClientInfo &info) {
    Message msg;
    Wolf &instance = GetInstance();
    if ((ans.status == Status::ALIVE && abs(instance.m_curr_num - ans.number) <= 70 / instance.m_clientsNum) ||
        (ans.status == Status::DEAD && abs(instance.m_curr_num - ans.number) <= 20 / instance.m_clientsNum)) {
        info.count_dead = 0;
    } else {
        msg.status = Status::DEAD;
        info.count_dead++;
    }
    msg.number = instance.m_curr_num;
    return msg;
}


void Wolf::GetNumber() {
    std::cout << "Input new host number: " << std::endl;

    int num = -1;
    while (num == -1) {
        std::cin >> num;

        if (!(std::cin.fail())) {
            if (num >= 1 && num <= RAND_LIMIT)
                m_curr_num = num;
            else {
                std::cout << "Input should be an integer in range [1; " << RAND_LIMIT << "]. Please, try again..."
                          << std::endl;
                num = -1;
            }
        } else {
            num = -1;
            std::cout << "Input should be an integer in range [1; " << RAND_LIMIT << "]. Please, try again..."
                      << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        }
    }
}

int Wolf::GetClientId() {
    static int i = 0;
    return i != m_clientsNum ? i++ : -1;
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr) {
    static Wolf &instance = GetInstance();
    switch (signum) {
        case SIGUSR1: {
            int iClient = instance.GetClientId();
            if (iClient == -1) {
                std::cout << "Too many clients" << std::endl;
            } else {

                union sigval value;
                value.sival_int = iClient;
                if (sigqueue(info->si_pid, SIGUSR1, value) == 0) {
                    std::cout << "Attaching client with pid: " << info->si_pid << " and id: " << iClient << std::endl;
                    instance.m_client_info[iClient].Attach(info->si_pid);
                }
            }
            break;
        }
        case SIGUSR2: {
            for (int i = 0; i < instance.m_clientsNum; ++i)
                if (instance.m_client_info[i].pid == info->si_pid && instance.m_client_info[i].attached) {
                    instance.Terminate(SIGINT);
                    break;
                }
            break;
        }
        default: {
            if (getpid() != info->si_pid) {
                instance.Terminate(signum);
            }
        }
    }
}

void *Wolf::ThreadRun(void *pthrData) {
    ClientInfo *info = (ClientInfo *) pthrData;
    Wolf &instance = Wolf::GetInstance();
    int id = info->id;
    sem_t *semaphore_host = info->semaphore_host;
    sem_t *semaphore_client = info->semaphore_client;
    Conn connection = info->connection;

    int clientStep = 0;
    struct timespec ts;
    Message msg;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    instance.m_finished++;
    while (true) {
        if (info->attached) {
            pthread_mutex_lock(&instance.m_mutx);
            while (clientStep >= instance.m_step)
                pthread_cond_wait(&instance.m_cond, &instance.m_mutx);
            clientStep++;

            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            if (sem_timedwait(semaphore_host, &ts) == -1) {
                std::cout << "Time wait id:" << id << std::endl; //debug
                instance.Terminate(SIGTERM);
            }
            if (connection.Read(&msg, sizeof(Message))) {
                std::cout << "--------------------------------" << std::endl;
                std::cout << "Goat[" << id << "] current status: "
                          << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;
                std::cout << "Goat[" << id << "] number: " << msg.number << std::endl;
                msg = instance.Step(msg, *info);
                std::cout << "Goat[" << id << "] new status: "
                          << ((msg.status == Status::ALIVE) ? "alive" : "dead") << std::endl;

                connection.Write(&msg, sizeof(msg));
            }
            sem_post(semaphore_client);
            ++instance.m_finished;
            pthread_mutex_unlock(&instance.m_mutx);
        } else
            pthread_exit(nullptr);
    }
}

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "wolf.h"
#include "goat.h"
#include <fcntl.h>
#include <csignal>
#include "conn.h"
#include <semaphore.h>
#include <random>
#include <list>


wolf *wolf::instance = new wolf();

wolf::wolf() {}

wolf *wolf::get_instance() {
    return instance;
}


void *wolf::ReadGoat(void *param) {
    struct timespec ts;
    goat *currGoat = (goat *) param;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIMEOUT;
    message buf(Owner::WOLF);

    while (true) {
        if (sem_timedwait(currGoat->getSemaphore(), &ts) != -1) {
            if (currGoat->getConnection()->Read(&buf)) {
                if (buf.owner != Owner::WOLF) {
                    std::cout << "goat number " << buf.number << std::endl;
                    break;
                } else {
                    currGoat->getConnection()->Write(&buf);
                }
                sem_post(currGoat->getSemaphore());
            }
        } else {

            kill(currGoat->getPid(), SIGTERM);
            currGoat->getConnection()->Close();
            instance->clients.erase(currGoat);
            instance->numGoats--;
            return nullptr;
        }
    }
    instance->clients[currGoat] = buf;
    return nullptr;
}

void *wolf::WriteGoat(void *param) {
    goat *currGoat = (goat *) param;
    Status st = instance->clients[currGoat].status;

    if (st == Status::ALIVE &&
        abs(instance->wolfNumber - instance->clients[currGoat].number) <= 70 / instance->numGoats) {
        st = Status::ALIVE;
    } else if (st == Status::DEAD &&
               abs(instance->wolfNumber - instance->clients[currGoat].number) <= 20 / instance->numGoats) {
        st = Status::ALIVE;
    } else {
        st = Status::DEAD;
    }
    message msg(Owner::WOLF, instance->wolfNumber, st);

    currGoat->getConnection()->Write(&msg, sizeof(msg));
    sem_post(currGoat->getSemaphore());
    instance->clients[currGoat] = msg;
    return nullptr;
}

void wolf::CreateGoats() {
    while (true) {
        std::cout << "Enter number of goats: ";
        std::cin >> numGoats;
        if (numGoats > 0)
            break;
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        std::cout << "Incorrect number, number must be >0" << std::endl;
    }
    std::cout << "Start creating goats" << std::endl;
    for (int i = 0; i < numGoats; i++) {
        conn *currentConnection = new conn();
        currentConnection->Open((size_t) i);
        sem_t *semClient = new sem_t();
        message msg(Owner::WOLF);
        if ((semClient = sem_open(("/sem_con" + std::to_string(i)).c_str(), O_CREAT, 0666, 1)) != SEM_FAILED) {
            int val;
            sem_getvalue(semClient, &val);
            if (val == 0) {
                sem_post(semClient);
            }
            currentConnection->Write(&msg, sizeof(msg));
        }
        goat *currGoat = new goat(Status::ALIVE, currentConnection, semClient);
        int pid = fork();
        if (pid == 0) {
            currGoat->start();
            return;
        }
        currGoat->setPid(pid);
        clients.insert(std::pair<goat *, message>(currGoat, msg));
    }
    std::cout << "Finish creating goats" << std::endl << std::endl;

}

void wolf::threadsWork(void *(*pFunction)(void *)) {
    std::list <pthread_t> tids;
    for (auto iter : clients) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, pFunction, iter.first);
        tids.push_back(tid);
    }
    for (auto tid : tids) {
        pthread_join(tid, nullptr);
    }
}

void wolf::start() {
    CreateGoats();
    int prevAlive = numGoats;
    int numAlive = 0;
    while (true) {

        threadsWork(wolf::ReadGoat);

        while (true) {
            std::cout << "Enter wolf's number: ";
            std::cin >> wolfNumber;
            if (wolfNumber > 0 && wolfNumber <= 100)
                break;
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "Incorrect number, number must be >0 and <=100" << std::endl;
        }

        threadsWork(wolf::WriteGoat);

        numAlive = 0;
        for (auto iter : clients) {
            if (iter.second.status == Status::ALIVE)
                numAlive++;
        }
        std::cout << "alive/numGoats: " << numAlive << "/" << numGoats << std::endl;
        if (numAlive == 0 && prevAlive == 0) {
            std::cout << "Game over" << std::endl;
            for (auto iter:clients) {
                iter.first->getConnection()->Close();
                kill(iter.first->getPid(), SIGTERM);
            }
            exit(EXIT_SUCCESS);
        }
        prevAlive = numAlive;
        std::cout << std::endl;
    }
}
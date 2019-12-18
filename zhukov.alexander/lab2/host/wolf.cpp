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


int wolf::numGoats = 5;
int wolf::wolfNumber = 5;
wolf *wolf::instance = new wolf();
std::map<conn *, goat *> wolf::connections = std::map<conn *, goat *>();
std::map<conn *, message> wolf::messages = std::map<conn *, message>();
std::map<conn *, Status> wolf::statuses = std::map<conn *, Status>();


wolf::wolf() {}

wolf *wolf::get_instance() {
    return instance;
}


void *wolf::ReadGoat(void *param) {
    struct timespec ts;
    conn *connection = (conn *) param;
    clock_gettime(CLOCK_REALTIME, &ts);
    __time_t lastTime = ts.tv_sec + TIMEOUT;
    __time_t currentTime = ts.tv_sec;
    message buf(WOLF);

    while (currentTime < lastTime) {
        currentTime = ts.tv_sec;
        if (sem_trywait(connections[connection]->getSemaphore()) != -1) {
            if (connection->Read(&buf)) {
                if (buf.owner != WOLF) {
                    std::cout << "goat number " << buf.number << std::endl;
                    break;
                } else {
                    connection->Write(&buf);
                }
                sem_post(connections[connection]->getSemaphore());
            }
        } else {
            continue;
        }
    }
    if (currentTime >= lastTime) {
        kill(connections[connection]->getPid(), SIGTERM);
        connection->Close();
        connections.erase(connection);
        numGoats--;
        return nullptr;
    }
    messages[connection] = buf;
    return nullptr;
}

void *wolf::WriteGoat(void *param) {
    conn *connection = (conn *) param;
    Status st = messages[connection].status;

    if (st == ALIVE && abs(wolfNumber - messages[connection].number) <= 70 / numGoats) {
        st = ALIVE;
        statuses[connection] = ALIVE;
    } else if (st == DEAD && abs(wolfNumber - messages[connection].number) <= 20 / numGoats) {
        st = ALIVE;
        statuses[connection] = ALIVE;
    } else {
        st = DEAD;
        statuses[connection] = DEAD;
    }
    message msg(WOLF, wolfNumber, st);

    connection->Write(&msg, sizeof(msg));
    sem_post(connections[connection]->getSemaphore());
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
        if ((semClient = sem_open(("/sem_con" + std::to_string(i)).c_str(), O_CREAT, 0666, 1)) !=
            SEM_FAILED) {
            int val;
            sem_getvalue(semClient, &val);
            if (val == 0) {
                sem_post(semClient);
            }
            message msg(WOLF);
            currentConnection->Write(&msg, sizeof(msg));
        }
        goat *currGoat = new goat(ALIVE, currentConnection, semClient);
        int pid = fork();
        if (pid == 0) {
            currGoat->start();
            return;
        }
        currGoat->setPid(pid);
        connections.insert(std::pair<conn *, goat *>(currentConnection, currGoat));
    }
    std::cout << "Finish creating goats" << std::endl;

}


void wolf::start() {
    CreateGoats();
    int prevAlive = numGoats;
    int numAlive = 0;
    while (true) {

        std::list <pthread_t> tids;

        for (auto iter : connections) {
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tid, &attr, ReadGoat, iter.first);
            tids.push_back(tid);
        }
        for (auto tid : tids) {
            pthread_join(tid, nullptr);
        }

        while (true) {
            std::cout << "Enter wolf's number: ";
            std::cin >> wolfNumber;
            if (wolfNumber > 0 && wolfNumber <= 100)
                break;
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "Incorrect number, number must be >0 and <=100" << std::endl;
        }

        tids.clear();
        for (auto iter : connections) {
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tid, &attr, WriteGoat, iter.first);
            tids.push_back(tid);
        }
        for (auto tid : tids) {
            pthread_join(tid, nullptr);
        }


        numAlive = 0;
        for (auto iter : connections) {
            if (statuses[iter.first] == ALIVE)
                numAlive++;
        }
        std::cout << "alive/numGoats: " << numAlive << "/" << numGoats << std::endl;
        if (numAlive == 0 && prevAlive == 0) {
            std::cout << "Game over" << std::endl;
            for (auto iter:connections) {
                iter.first->Close();
                kill(iter.second->getPid(), SIGTERM);
            }
            exit(EXIT_SUCCESS);
        }
        prevAlive = numAlive;
        std::cout << std::endl;
    }
}
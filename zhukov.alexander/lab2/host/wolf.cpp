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


int numGoats = 5;
int wolfNumber = 5;
wolf *wolf::instance = new wolf();
std::map<conn *, connection_info_wolf *> wolf::connections = std::map<conn *, connection_info_wolf *>();


wolf::wolf() {}

wolf *wolf::get_instance() {
    return instance;
}


void *wolf::work(void *param) {
    struct timespec ts;
    conn *connection = (conn *) param;
    clock_gettime(CLOCK_REALTIME, &ts);
    __time_t lastTime = ts.tv_sec + TIMEOUT;
    __time_t currentTime = ts.tv_sec;
    message buf(WOLF);

    while (currentTime < lastTime) {
        currentTime = ts.tv_sec;
        if (sem_trywait(connections[connection]->sem) != -1) {
            connection->Read(&buf);
            if (buf.owner != WOLF) {
                sem_post(connections[connection]->sem);
                break;
            } else {
                connection->Write(&buf);
            }
            sem_post(connections[connection]->sem);
        } else {
            continue;
        }
    }
    if (currentTime >= lastTime) {
        kill(connections[connection]->pid, SIGTERM);
        connection->Close();
        connections.erase(connection);
        numGoats--;
        return nullptr;
    }


    Status st = buf.status;
    std::cout << "goat number " << buf.number << std::endl;
    if (st == ALIVE && abs(wolfNumber - buf.number) <= 70 / numGoats) {
        st = ALIVE;
        connections[connection]->goatStatus = ALIVE;
    } else if (st == DEAD && abs(wolfNumber - buf.number) <= 20 / numGoats) {
        st = ALIVE;
        connections[connection]->goatStatus = ALIVE;
    } else {
        st = DEAD;
        connections[connection]->goatStatus = DEAD;
    }
    message msg(WOLF, wolfNumber, st);


    sem_wait(connections[connection]->sem);
    connection->Write(&msg, sizeof(msg));
    sem_post(connections[connection]->sem);
    return nullptr;
}

void wolf::start() {
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
        connection_info_wolf *connectionsInfoWolf = new connection_info_wolf();
        conn *currentConnection = new conn();
        currentConnection->Open((size_t) i);
        sem_t *semClient = new sem_t();
        if ((semClient = sem_open(("/sem_con" + std::to_string(i)).c_str(), O_CREAT, 0666, 1)) !=
            SEM_FAILED) {
            connectionsInfoWolf->sem = semClient;
            connectionsInfoWolf->goatStatus = ALIVE;
            int *val = new int;
            sem_getvalue(semClient, val);
            if (*val == 0) {
                sem_post(connections[currentConnection]->sem);
            }
            message msg(WOLF);
            currentConnection->Write(&msg, sizeof(msg));
        }
        connection_info_goat conInfo(currentConnection, semClient);
        int pid = fork();
        if (pid == 0) {
            goat::start(conInfo);
            return;
        }
        connectionsInfoWolf->pid = pid;
        connections.insert(std::pair<conn *, connection_info_wolf *>(currentConnection, connectionsInfoWolf));
    }
    std::cout << "Finish creating goats" << std::endl;

    int prevAlive = numGoats;
    int numAlive = 0;
    while (true) {

        std::list<pthread_t> tids;
        while (true) {
            std::cout << "Enter wolf's number: ";
            std::cin >> wolfNumber;
            if (wolfNumber > 0 && wolfNumber <= 100)
                break;
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "Incorrect number, number must be >0 and <=100" << std::endl;
        }

        for (auto iter : connections) {
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tid, &attr, work, iter.first);
            tids.push_back(tid);
        }
        for (auto tid : tids) {
            pthread_join(tid, nullptr);
        }
        numAlive = 0;
        for (auto iter : connections) {
            if (connections[iter.first]->goatStatus == ALIVE)
                numAlive++;
        }
        std::cout << "alive/numGoats: " << numAlive << "/" << numGoats << std::endl;
        if (numAlive == 0 && prevAlive == 0) {
            std::cout << "Game over" << std::endl;
            for (auto iter:connections) {
                iter.first->Close();
                kill(connections[iter.first]->pid, SIGTERM);
            }
            exit(EXIT_SUCCESS);
        }
        prevAlive = numAlive;
        std::cout << std::endl;
    }
}
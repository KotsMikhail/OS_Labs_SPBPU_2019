#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include "goat.h"
#include <random>


Status goat::status = ALIVE;

int goat::start(const connection_info_goat &connectionInfo) {
    while (true) {
        sem_wait(connectionInfo.semaphore);
        message *buf = new message();
        connectionInfo.connection->Read(buf);
        if (buf->owner == WOLF) {
            status = buf->status;
            int num = GenerateRandNum(status == ALIVE ? 100 : 50);
            buf = new message(GOAT, num, status);
            connectionInfo.connection->Write(buf);
        } else {
            connectionInfo.connection->Write(buf);
        }
        sem_post(connectionInfo.semaphore);
        sleep(1);
    }
}

int goat::GenerateRandNum(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}
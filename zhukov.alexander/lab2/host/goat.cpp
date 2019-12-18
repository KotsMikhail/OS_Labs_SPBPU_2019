#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include "goat.h"
#include <random>


int goat::start() {
    while (true) {
        sem_wait(getSemaphore());
        message *buf = new message();
        if (connection->Read(buf)) {
            if (buf->owner == WOLF) {
                status = buf->status;
                int num = GenerateRandNum(status == ALIVE ? 100 : 50);
                buf = new message(GOAT, num, status);
                connection->Write(buf);
            } else {
                connection->Write(buf);
            }
        }
        sem_post(getSemaphore());
        sleep(1);
    }
}

int goat::GenerateRandNum(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}

Status goat::getStatus() { return status; }

sem_t *goat::getSemaphore() { return semaphore; }

pid_t goat::getPid() { return pid; }
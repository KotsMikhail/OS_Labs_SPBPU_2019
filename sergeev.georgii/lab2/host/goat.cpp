//
// Created by nero on 22.11.2019.
//

#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include "goat.h"
#include <random>


void goat::start() {
    while (true) {
        sem_wait(semaphore);
        message *buf = new message();
        if (!connection->Read(buf)){
            break;
        }
        if (buf->owner == Owner::WOLF) {
            status = buf->status;
            int num = generate_rand_i(status == Status::ALIVE ? 100 : 50);
            buf = new message(Owner::GOAT, num, status);
        }
        if (!connection->Write(buf)){
            break;
        }
        sem_post(semaphore);
    }
}

int goat::generate_rand_i(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}
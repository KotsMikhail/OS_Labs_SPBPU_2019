//
// Created by nero on 22.11.2019.
//

#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include "goat.h"
#include "connection_info.h"
#include <random>


Status goat::status = ALIVE;

int goat::start(const connection_info &c_inf) {
    while (true) {
        sem_wait(c_inf.semaphore);
        message *buf = new message();
        c_inf.connection->Read(c_inf.id, buf);
        if (buf->owner == WOLF) {
            status = buf->status;
            int num = generate_rand_i(status == ALIVE ? 100 : 50);
            buf = new message(GOAT, num, status);
            c_inf.connection->Write(c_inf.id, buf);
        } else {
            c_inf.connection->Write(c_inf.id, buf);
        }
        sem_post(c_inf.semaphore);
    }
}

int goat::generate_rand_i(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}
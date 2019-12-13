//
// Created by nero on 21.11.2019.
//

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
#include "connection_info.h"
#include <list>


int num_goats = 0;
int num = 0;
wolf *wolf::instance = new wolf();
std::map<conn *, sem_t *>  wolf::connections = std::map<conn *, sem_t *>();
std::map<conn *, Status>  wolf::states = std::map<conn *, Status>();
std::map<conn *, int>  wolf::goats_id = std::map<conn *, int>();
std::map<conn *, pid_t>  wolf::pids = std::map<conn *, pid_t>();

wolf::wolf() {

}

wolf *wolf::get_instance() {
    return instance;
}

void *wolf::work(void *param) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    __time_t saved_time = ts.tv_sec + TIMEOUT;
    __time_t curr_time = ts.tv_sec;
    auto *sem = connections[(conn *) param];
    conn *connection = (conn *) param;
    message buf(WOLF);

    while (curr_time < saved_time) {
        curr_time = ts.tv_sec;
        if (sem_trywait(sem) != -1) {
            connection->Read(goats_id[connection], &buf);
            if (buf.owner != WOLF) {
                sem_post(sem);
                break;
            } else {
                connection->Write(goats_id[connection], &buf);
            }
            sem_post(sem);
        } else {
            continue;
        }
    }
    if (curr_time >= saved_time) {
        kill(pids[connection], SIGTERM);
        connections.erase(connection);
        num_goats--;
        return nullptr;
    }
    Status st = buf.status;
    std::cout << "goat " << buf.number << std::endl;
    if (st == ALIVE && abs(num - buf.number) <= 70 / num_goats) {
        st = ALIVE;
        states[connection] = ALIVE;
    } else if (st == DEAD && abs(num - buf.number) <= 20 / num_goats) {
        st = ALIVE;
        states[connection] = ALIVE;
    } else {
        st = DEAD;
        states[connection] = DEAD;
    }
    message msg(WOLF, num, st);
    sem_wait(sem);
    connection->Write(goats_id[connection], &msg, sizeof(msg));
    sem_post(sem);
    return nullptr;
}

void wolf::start() {
    std::cout << "Enter initial number of goats: ";
    while (true) {
        std::cin >> num_goats;
        if (num_goats > 0)
            break;
        std::cout << "Wrong input. Enter number greater than 0." << std::endl;
    }

    std::cout << "Create " << num_goats << "goats" << std::endl;
    for (int i = 0; i < num_goats; i++) {
        conn *current_connection = new conn();
        current_connection->Open((size_t) i);
        sem_t *semaphore_client = new sem_t();
        if ((semaphore_client = sem_open(("/sem_conn" + std::to_string(i)).c_str(), O_CREAT, 0666, 1)) !=
            SEM_FAILED) {
            connections.insert(std::pair<conn *, sem_t *>(current_connection, semaphore_client));
            goats_id.insert(std::pair<conn *, int>(current_connection, i));
            states.insert(std::pair<conn *, Status>(current_connection, DEAD));
            int *val = new int;
            sem_getvalue(semaphore_client, val);
            if (*val == 0) {
                sem_post(connections[current_connection]);
            }
            message msg(WOLF); // initial message
            sem_wait(connections[current_connection]);
            current_connection->Write(i, &msg, sizeof(msg));
            sem_post(connections[current_connection]);
        }
        connection_info c_inf = connection_info(current_connection, semaphore_client, (size_t) i);
        int pid = fork();
        if (pid == 0) {
            goat::start(c_inf);
            return;
        }
        pids.insert(std::pair<conn *, pid_t>(current_connection, pid));
    }
    std::cout << "Game starts" << std::endl << std::endl;
    int prev_dead = 0;
    int num_dead = 0;
    while (true) {
        std::cout << "Enter wolf's number: ";
        while (true) {
            std::cin >> num;
            if (num > 0 && num <= 100) {
                break;
            }
            std::cout << "Wrong input. Enter number between 1 and 100." << std::endl;
        }

        std::list<pthread_t> tids;
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
        num_dead = 0;
        for (auto iter : connections) {
            if (states[iter.first] == DEAD)
                num_dead++;
        }
        std::cout << "dead/all: " << num_dead << "/" << num_goats << std::endl << std::endl;
        if (num_dead == num_goats && num_goats == prev_dead) {
            std::cout << "Wolf win" << std::endl;
            for (auto iter:connections) {
                kill(pids[iter.first], SIGTERM);
            }
            exit(EXIT_SUCCESS);
        }
        prev_dead = num_dead;
    }
}

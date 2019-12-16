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


int wolf::num_goats = 0;
int wolf::num = 0;
wolf *wolf::instance = new wolf();
std::map<conn *, sem_t *>  wolf::connections = std::map<conn *, sem_t *>();
std::map<conn *, Status>  wolf::states = std::map<conn *, Status>();
std::map<conn *, int>  wolf::goats_id = std::map<conn *, int>();
std::map<conn *, pid_t>  wolf::pids = std::map<conn *, pid_t>();
std::map<conn *, message>  wolf::goats_messages = std::map<conn *, message>();

wolf::wolf() {

}

wolf *wolf::get_instance() {
    return instance;
}

void *wolf::catch_goats(void *param) {
    struct timespec ts;
    struct timespec ts_work;
    clock_gettime(CLOCK_REALTIME, &ts);
    __time_t saved_time = ts.tv_sec + TIMEOUT;
    __time_t curr_time = ts.tv_sec;
    auto *sem = connections[(conn *) param];
    conn *connection = (conn *) param;
    message buf(WOLF);

    while (curr_time < saved_time) {
        curr_time = ts.tv_sec;
        clock_gettime(CLOCK_REALTIME, &ts_work);
        ts_work.tv_sec += WOLF_WORK_WINDOW;
        if (sem_timedwait(sem, &ts_work) != -1) {
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
    goats_messages[connection] = buf;
    return nullptr;
}

void *wolf::eat_goats(void *param) {
    struct timespec ts;
    auto *sem = connections[(conn *) param];
    conn *connection = (conn *) param;
    message buf = goats_messages[connection];
    Status st = buf.status;
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
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WOLF_WORK_WINDOW;
    sem_timedwait(sem, &ts);
    connection->Write(goats_id[connection], &msg, sizeof(msg));
    sem_post(sem);
    return nullptr;
}

void wolf::start() {
    init_game();
    start_game();
}

void wolf::init_game() {
    std::cout << "Enter initial number of goats: ";
    while (true) {
        std::cin >> num_goats;
        if (std::cin.fail()){
            std::cin.clear();
            std::cin.ignore(256, '\n');
        }
        else if (num_goats > 0)
            break;
        std::cout << "Wrong input. Enter number greater than 0." << std::endl;
    }
    std::string goat_s = num_goats == 1 ? "goat" : "goats";
    std::cout << "Create " << num_goats << " " << goat_s << std::endl;
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
}

void wolf::start_game() {
    std::cout << "Game starts" << std::endl << std::endl;
    int prev_dead = 0;
    int num_dead = 0;
    while (true) {
        // work-read
        std::list <pthread_t> tids;
        for (auto iter : connections) {
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tid, &attr, catch_goats, iter.first);
            tids.push_back(tid);
        }
        for (auto tid : tids) {
            pthread_join(tid, nullptr);
        }
        //print goats turn
        for (auto iter : connections) {
            std::string goat_stat = goats_messages[iter.first].status == ALIVE ? "A" : "D";
            std::cout << "goat " << goats_messages[iter.first].number << " status: " << goat_stat << std::endl;
        }
        // wolf's turn
        std::cout << "Enter wolf's number: ";
        while (true) {
            std::cin >> num;
            if (std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(256, '\n');
            }
            else if (num > 0 && num <= 100) {
                break;
            }
            std::cout << "Wrong input. Enter number between 1 and 100." << std::endl;
        }
        // work-write
        tids.clear();
        for (auto iter : connections) {
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&tid, &attr, eat_goats, iter.first);
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

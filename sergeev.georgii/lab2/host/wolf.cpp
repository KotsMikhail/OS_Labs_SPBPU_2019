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
#include <list>


wolf *wolf::instance = new wolf();
std::map<conn*, goat*> wolf::clients = std::map<conn*, goat*>();
std::map<conn*, message> wolf::clients_messages = std::map<conn*, message>();
int wolf::num = 0;
int wolf::num_goats = 0;

wolf::wolf() {
}

wolf *wolf::get_instance() {
    return instance;
}

void *wolf::catch_goats(void *param) {
    struct timespec ts;
    struct timespec ts_work;
    bool error = false;
    clock_gettime(CLOCK_REALTIME, &ts);
    __time_t saved_time = ts.tv_sec + TIMEOUT;
    __time_t curr_time = ts.tv_sec;
    conn *connection = (conn *) param;
    auto *sem = clients[connection]->get_semaphore();
    message buf(Owner::WOLF);
    while (curr_time < saved_time) {
        curr_time = ts.tv_sec;
        clock_gettime(CLOCK_REALTIME, &ts_work);
        ts_work.tv_sec += WOLF_WORK_WINDOW;
        if (sem_timedwait(sem, &ts_work) != -1) {
            if (!connection->Read(&buf)){
                error = true;
                break;
            }
            if (buf.owner != Owner::WOLF) {
                sem_post(sem);
                clients_messages[connection] = buf;
                break;
            } else {
                if (!connection->Write(&buf)){
                    error = true;
                    break;
                }
            }
            sem_post(sem);
        } else {
            continue;
        }
    }
    if (curr_time >= saved_time || error) {
        kill_client(connection);
        return nullptr;
    }
    return nullptr;
}

void *wolf::eat_goats(void *param) {
    struct timespec ts;
    conn *connection = (conn *) param;
    auto *sem = clients[connection]->get_semaphore();
    int client_number = clients_messages[connection].number;
    Status st = clients_messages[connection].status;
    if (st == Status::ALIVE && abs(num - client_number) <= 70 / num_goats) {
        st = Status::ALIVE;
        clients_messages[connection].status = Status::ALIVE;
    } else if (st == Status::DEAD && abs(num - client_number) <= 20 / num_goats) {
        st = Status::ALIVE;
        clients_messages[connection].status = Status::ALIVE;
    } else {
        st = Status::DEAD;
        clients_messages[connection].status = Status::DEAD;
    }
    message msg(Owner::WOLF, num, st);
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WOLF_WORK_WINDOW;
    sem_timedwait(sem, &ts);
    if (!connection->Write(&msg, sizeof(msg))){
        kill_client(connection);
        return nullptr;
    }
    sem_post(sem);
    return nullptr;
}

void wolf::kill_client(conn* connection){
    kill(clients[connection]->get_pid(), SIGTERM);
    clients.erase(connection);
    num_goats--;
}

void wolf::process_threads(void *(*start_routine) (void *)){
    std::list <pthread_t> tids;
    for (auto iter : clients) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, start_routine, iter.first);
        tids.push_back(tid);
    }
    for (auto tid : tids) {
        pthread_join(tid, nullptr);
    }
}
void wolf::start() {
    init_game();
    start_game();
}

void wolf::init_game() {
    std::cout << "Enter initial number of goats: ";
    while (true) {
        std::cin >> num_goats;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(256, '\n');
        } else if (num_goats > 0)
            break;
        std::cout << "Wrong input. Enter number greater than 0." << std::endl;
    }
    std::string goat_s = num_goats == 1 ? "goat" : "goats";
    std::cout << "Create " << num_goats << " " << goat_s << std::endl;
    for (int i = 0; i < num_goats; i++) {
        conn *current_connection = new conn();
        if (!current_connection->Open((size_t) i)) {
            continue;
        }
        sem_t *semaphore_client = new sem_t();
        if ((semaphore_client = sem_open(("/sem_conn" + std::to_string(i)).c_str(), O_CREAT, 0666, 1)) == SEM_FAILED) {
            std::cout << "Error on semaphore creating for client " << i << std::endl;
            continue;
        }
        int val;
        sem_getvalue(semaphore_client, &val);
        if (val == 0) {
            sem_post(semaphore_client);
        }
        message msg(Owner::WOLF); // initial message
        sem_wait(semaphore_client);
        if (!current_connection->Write(&msg, sizeof(msg))){
            continue;
        }
        sem_post(semaphore_client);
        goat* client = new goat(current_connection, semaphore_client);
        int pid = fork();
        if (pid == 0) {
            client->start();
            return;
        }
        client->set_pid(pid);
        clients.insert(std::pair<conn *, goat*>(current_connection, client));
    }
}


void wolf::start_game() {
    std::cout << "Game starts" << std::endl << std::endl;
    int prev_dead = 0;
    int num_dead = 0;
    while (true) {
        // work-read
       process_threads(wolf::catch_goats);
        //print goats turn
        for (auto iter : clients) {
            std::string goat_stat = clients_messages[iter.first].status == Status::ALIVE ? "A" : "D";
            std::cout << "goat " << clients_messages[iter.first].number << " status: " << goat_stat << std::endl;
        }
        // wolf's turn
        std::cout << "Enter wolf's number: ";
        while (true) {
            std::cin >> num;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(256, '\n');
            } else if (num > 0 && num <= 100) {
                break;
            }
            std::cout << "Wrong input. Enter number between 1 and 100." << std::endl;
        }
        // work-write
        process_threads(wolf::eat_goats);
        // process turn results
        num_dead = 0;
        for (auto iter : clients) {
            if (clients_messages[iter.first].status == Status::DEAD)
                num_dead++;
        }
        std::cout << "dead/all: " << num_dead << "/" << num_goats << std::endl << std::endl;
        if (num_dead == num_goats && num_goats == prev_dead) {
            std::cout << "Wolf win" << std::endl;
            for (auto iter : clients) {
                iter.first->Close();
                kill(iter.second->get_pid(), SIGTERM);
            }
            clients.clear();
            exit(EXIT_SUCCESS);
        }
        prev_dead = num_dead;
    }
}

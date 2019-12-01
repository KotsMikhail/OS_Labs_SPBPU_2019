//
// Created by misha on 23.11.2019.
//

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>
#include <fcntl.h>
#include <csignal>
#include <random>
#include <syslog.h>
#include "Oracle.h"
#include "my_functions.h"

Oracle::Oracle(int host_pid_) {
    //std::cout << "host handler pid is: " << host_pid_ << std::endl;
    syslog(LOG_NOTICE, "host handler pid is: %d", host_pid);
    host_pid = host_pid_;
    srand(time(NULL) + host_pid_);
    rand_offset = rand();
    signal(SIGTERM, SignalHandler);
}
Oracle::Oracle(Oracle& other){}
Oracle& Oracle::operator=(Oracle& other) {
    return other;
}

Oracle* Oracle::GetInstance(int host_pid_) {
    static Oracle self(host_pid_);
    return &self;
}


void Oracle::Start() {
    //std::cout << "I am client! pid: " << getpid() << std::endl;
    syslog(LOG_NOTICE, "I am client!");
    Message msg;
    Message buf;
    //int i = 1;
    while (true) {
        sem_wait(semaphore_client);
        if (connection.Read(&buf, sizeof(buf))) {
            msg.number = GetWeather(buf.day, buf.month, buf.year);
            //sleep(i++);
            connection.Write((void *) &msg, sizeof(msg));
            sem_post(semaphore_host);
        }
    }
}


bool Oracle::OpenConnection() {
    if (connection.Open(host_pid, false)) {
        sem_client_name = sem_name + "_client_" + toString(host_pid);
        sem_host_name = sem_name + "_host_" + toString(host_pid);


        semaphore_host = sem_open(sem_host_name.c_str(), 0);
        if (semaphore_host == SEM_FAILED) {
            //std::cout << "ERROR: client: can`t open host semaphore ("<< sem_host_name << ") error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: client: can`t open host semaphore (%s) with error %s", sem_host_name.c_str(),
                   strerror(errno));
            return false;
        }
        //std::cout << "client: host semaphore opened (" << sem_host_name << ")" << std::endl;
        syslog(LOG_NOTICE, "client: host semaphore opened (%s)", sem_host_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), 0);
        if (semaphore_client == SEM_FAILED) {
            //std::cout << "ERROR: client: can`t open client semaphore ("<< sem_client_name << ") error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: client: can`t open client semaphore (%s) with error %s", sem_client_name.c_str(),
                   strerror(errno));
            return false;
        }
        //std::cout << "client: client semaphore opened (" << sem_client_name << ")" << std::endl;
        syslog(LOG_NOTICE, "client: host semaphore opened (%s)", sem_client_name.c_str());

        //std::cout << "client pid: " << getpid() << std::endl;
        syslog(LOG_NOTICE, "client pid: %d", getpid());
        return true;
    }
    return false;
}

void Oracle::Terminate(int signum) {
    //std::cout << "Client::Terminate("<< getpid() << ")" << std::endl;
    syslog(LOG_NOTICE, "Client::Terminate(%d)", getpid());
    if (connection.Close()) {
        if (semaphore_host != SEM_FAILED) {
            semaphore_host = SEM_FAILED;
            sem_close(semaphore_host);
        }
        if (semaphore_client != SEM_FAILED) {
            semaphore_client = SEM_FAILED;
            sem_close(semaphore_client);
        }
        exit(signum);
    }
    //std::cout << "Terminating error: " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "Terminating error: %s", strerror(errno));
    exit(errno);
}


int Oracle::GetWeather(int day, int month, int year) {
    srand((day * 31 + month * 12 + year * 366 + rand_offset) % INTMAX_MAX);
    int temp = rand() % 50;
    int sig = (rand() % 2 == 0) ? -1 : 1;
    return temp * sig;
}


void Oracle::SignalHandler(int signum) {
    Oracle *instance = Oracle::GetInstance(30);
    switch (signum) {
        case SIGTERM: {
            instance->Terminate(signum);
        }
    }
}

void Oracle::SetPipe(Conn connection_) {
    connection = connection_;
}

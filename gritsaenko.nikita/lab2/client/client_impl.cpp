#include "client.h"
#include <iostream>
#include <random>
#include <syslog.h>
#include <cstring>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <string>
#include <vector>
#include <fcntl.h>

client_t::client_t() {}

client_t::~client_t() {}

client_t::client_t(int host_pid) : host_pid(host_pid) {
    struct sigaction sig_action;

    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = signal_handler;

    sigaction(SIGTERM, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);
    sigaction(SIGUSR1, &sig_action, NULL);

    is_connected = false;
}


client_t& client_t::get_instance(int host_pid) {
    static client_t instance(host_pid);
    return instance;
}


int client_t::connect() {
    kill(host_pid, SIGUSR1);
    
    return 0;
}


int client_t::disconnect() {
    semaphore_client = SEM_FAILED;
    semaphore_host = SEM_FAILED;
    sem_close(semaphore_host);
    sem_close(semaphore_client);
    return connection.destroy_connection();
}

int client_t::send_temperature(message_t *msg) {
    unsigned int hash = msg->day * 1000000 + msg->month * 10000 + msg->year;
    std::minstd_rand generator(hash);
    std::uniform_int_distribution<int> distribution(MIN_TEMP, MAX_TEMP);
    msg->temperature = distribution(generator);

    if (connection.write(msg, sizeof(message_t)) != 0)
        return -1;
    return 0;
}

message_t* client_t::receive_date() {
    message_t* m = new message_t();
    if (connection.read(m, sizeof(message_t)) != 0)
        return NULL;

    std::cout << "Client received date " << m->day << "." << m->month << "." << m->year << std::endl;
    syslog(LOG_INFO, "Client received date %i.%i.%i", m->day, m->month, m->year);

    return m;
}

void client_t::terminate() {
    kill(host_pid, SIGUSR2);
    is_connected = false;

    closelog();

    if (disconnect() == 0)
        exit(EXIT_SUCCESS);
    exit(EXIT_FAILURE);  
}

int client_t::connect_with_host() {
    if ((semaphore_client = sem_open(SEMAPHORE_CLIENT, 0666)) == SEM_FAILED) {
        syslog(LOG_ERR, "Error: can't open client semaphore");
        return -1;
    }

    if ((semaphore_host = sem_open(SEMAPHORE_HOST, 0666)) == SEM_FAILED) {
        syslog(LOG_ERR, "Error: can't open host semaphore");
        return -1;
    }

    if (connection.create_connection(false, true) != 0) {
        syslog(LOG_ERR, "Error: failed to create connection");
        return -1;
    }

    return 0;
}

void client_t::signal_handler(int signum, siginfo_t *info, void *context) {
    client_t &client = get_instance(info->si_pid);

    switch (signum)
    {
    case SIGTERM:
        syslog (LOG_NOTICE, "SIGTERM signal caught.");
        client.terminate();
        break;
    case SIGINT:
        syslog (LOG_NOTICE, "SIGINT signal caught.");
        client.terminate();
        break;
    case SIGUSR1:
        if (info->si_pid == client.host_pid) {
            if (client.connect_with_host() == 0) {
                client.is_connected = true;
                std::cout << "Client connected successfully" << std::endl;
            }
        }
        break;
    
    default:
        break;
    }
}

void client_t::run() {
    message_t* msg;
    while (true) {
        if (!is_connected) 
            continue;  

        sem_wait(semaphore_client);

        if ((msg = receive_date()) != NULL) {
            send_temperature(msg);
        }

        sem_post(semaphore_host);
    }

}
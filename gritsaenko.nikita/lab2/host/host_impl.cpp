#include "host.h"
#include <iostream>
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


host_t& host_t::get_instance() {
    static host_t instance;
    return instance;
}

host_t::host_t() {
    struct sigaction sig_action;

    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = signal_handler;

    sigaction(SIGUSR1, &sig_action, NULL);
    sigaction(SIGUSR2, &sig_action, NULL);
    sigaction(SIGTERM, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);

    client_pid = -1;
}

host_t::~host_t() {}


int host_t::connect() {
    if ((semaphore_host = sem_open(SEMAPHORE_HOST, O_CREAT, 0666, 0)) == SEM_FAILED) {
        syslog(LOG_ERR, "Error: can't create host semaphore");
        return -1;
    }
    if ((semaphore_client = sem_open(SEMAPHORE_CLIENT, O_CREAT, 0666, 0)) == SEM_FAILED) {
        syslog(LOG_ERR, "Error: can't create client semaphore");
        sem_unlink(SEMAPHORE_HOST);
        return -1;
    }

    if (connection.create_connection(true, true) != 0) {
        syslog(LOG_ERR, "Error: failed to create connection");
        sem_unlink(SEMAPHORE_HOST);
        sem_unlink(SEMAPHORE_CLIENT);
        return -1;
    }

    return 0;
}

int host_t::disconnect() {
    semaphore_client = SEM_FAILED;
    semaphore_host = SEM_FAILED;
    sem_unlink(SEMAPHORE_HOST);
    sem_unlink(SEMAPHORE_CLIENT);
    client_pid = -1;
    return connection.destroy_connection();
}

std::vector<int> str_to_vector(std::string str)
{
	std::vector<int> v;
	std::istringstream date_stream(str);
	std::string s;
	while (std::getline(date_stream, s, '.')) {
		try {
            int n = std::stoi(s);
            v.push_back(n);
        }
        catch (std::exception &e) {
            std::cout << "Error: incorrect date, impossible to convert input to data" << std::endl;
        }
    }

	return v;
}

message_t* host_t::read_date() {
    std::string input;
    std::cout << "Input date in format 'dd.mm.yyyy': ";
    std::cin.clear();
    std::cin  >> input;
    std::cout << std::endl;

    std::vector<int> date = str_to_vector(input);

    if (date.size() < 3) {
        std::cout << "Incorrect date (too few data)" << std::endl;
        return NULL;
    }

    unsigned int day = date.at(0);
    unsigned int month = date.at(1);
    unsigned int year = date.at(2);

    if (day == 0 || day > 31 || month == 0 || month > 12) {
        std::cout << "Incorrect date (day or month out of range)" << std::endl;
        return NULL;
    }
    
    return new message_t(day, month, year);
}

int host_t::send_date() {
    message_t* message;
    
    if ((message = read_date()) == NULL)
        return -1;

    if (connection.write(message, sizeof(message_t)) != 0)
        return -1;

    curr_date = message;
    return 0;
}

int host_t::receive_temperature() {
    if (connection.read(curr_date, sizeof(message_t)) != 0)
        return -1;

    std::cout << "Client predicted temperature " << curr_date->temperature << std::endl;
    syslog(LOG_INFO, "Client predicted temperature %i", curr_date->temperature);

    return 0;
}

void host_t::terminate() {
    host_t &host = host_t::get_instance(); 
    if (host.client_pid != -1)
        kill(host.client_pid, SIGTERM);

    closelog();

    if (disconnect() == 0)
        exit(EXIT_SUCCESS);

    exit(EXIT_FAILURE);  
}

void host_t::signal_handler(int signum, siginfo_t *info, void *context) {
    host_t &host = host_t::get_instance();

    switch (signum)
    {
    case SIGTERM:
        syslog (LOG_NOTICE, "SIGTERM signal caught.");
        host.terminate();
        break;

    case SIGINT:
        syslog (LOG_NOTICE, "SIGINT signal caught.");
        host.terminate();
        break;

    case SIGUSR1:
        syslog (LOG_NOTICE, "SIGUSR1 signal caught.");
        if (host.client_pid != -1) {
            std::cout << "Host can be attached to only one client!" << std::endl;
            syslog (LOG_WARNING, "Host can be attached to only one client!");
        }
        else {
            std::cout << "Host attached client " << info->si_pid << std::endl;
            syslog (LOG_NOTICE, "Host attached client %i", info->si_pid);
            host.client_pid = info->si_pid;
            kill(info->si_pid, SIGUSR1);
        }
        break;

    case SIGUSR2:
        syslog (LOG_NOTICE, "SIGUSR2 signal caught.");
        if (host.client_pid == info->si_pid) {
            host.client_pid = -1;
        }
        break;

    default:
        break;
    }
}

void host_t::run() {
    while (true) {
        if (client_pid == -1) {
            continue;
        }

        if (send_date() != 0)
            continue;
        
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += TIMEOUT_SEC;

        sem_post(semaphore_client);

        if (sem_timedwait(semaphore_host, &ts) != 0) {
            std::cout << "Timeout!" << std::endl;
            kill(client_pid, SIGTERM);
            client_pid = -1;
            continue;
        }

        receive_temperature();
    }

}
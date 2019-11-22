#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <random>
#include <string>

#include "user_client.h"

client_t::client_t(int host_pid) : _host_pid(host_pid)
{
    _host_pid = host_pid;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signal_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
};

client_t::~client_t()
{
    syslog(LOG_NOTICE, "client: destroyed.");
}

client_t & client_t::get_instance(int host_pid)
{
    static client_t instance(host_pid);
    return instance;
}

void client_t::signal_handler(int sig, siginfo_t *info, void *context)
{
    client_t &instance = get_instance(0);
    switch (sig) 
    {
    case SIGTERM:
        syslog(LOG_NOTICE, "client: terminate signal catched.");
        if (info->si_pid != instance._host_pid)
            kill(instance._host_pid, SIGUSR1);
        if (instance.close_connection())
        {
            closelog();
            exit(EXIT_SUCCESS);
        }
        syslog(LOG_NOTICE, "client: couldn't close connection.");
        closelog();
        exit(EXIT_FAILURE);
        break;
    }
}

bool client_t::open_connection()
{
    _host_sem = sem_open(SEM_HOST_NAME, O_CREAT);
    if (_host_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "client: couldn't open host semaphore.");
        return false;
    }
    _client_sem = sem_open(SEM_CLIENT_NAME, O_CREAT);
    if (_client_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "client: couldn't open client semaphore.");
        sem_close(_host_sem);
        return false;
    }
    if (!_connection.conn_open(getpid(), false))
    {
        syslog(LOG_ERR, "client: couldn't open connection.");
        sem_close(_host_sem);
        sem_close(_client_sem);
        return false;
    }
    kill(_host_pid, SIGUSR1);
    return true;
}

bool client_t::close_connection()
{
    if (_host_sem != SEM_FAILED)
    {
        _host_sem = SEM_FAILED;
        sem_close(_host_sem);
    }
    if (_client_sem != SEM_FAILED)
    {
        _client_sem = SEM_FAILED;
        sem_close(_client_sem);
    }
    _connection.conn_close();
    return true;
}

void client_t::calculate_temperature(message_t &msg)
{
    unsigned long date = std::stoul(std::to_string(msg.day) + std::to_string(msg.mon) + std::to_string(msg.year));
    std::minstd_rand mt(date);
    std::uniform_int_distribution<int> temp(-91, 57);
    msg.temp = temp(mt);
    syslog(LOG_NOTICE, "client: temperature for date %u.%u.%u is %d.", msg.day, msg.mon, msg.year, msg.temp);
}

void client_t::run()
{
    message_t msg;
    while (true)
    {
        sem_wait(_client_sem);
        if (_connection.conn_recv(&msg, MESSAGE_SIZE))
        {
            calculate_temperature(msg);
            _connection.conn_send(&msg, MESSAGE_SIZE);
        }
        sem_post(_host_sem);
    }
}

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "user_host.h"

host_t::host_t()
{
    attach_client(0);
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signal_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
};

host_t::~host_t()
{
    syslog(LOG_NOTICE, "host: destroyed.");
}

void host_t::attach_client(int client_pid)
{
    _client_pid = client_pid;
    _is_client_attached = (client_pid != 0);
}

host_t & host_t::get_instance()
{
    static host_t instance;
    return instance;
}

void host_t::terminate()
{
    if (_is_client_attached)
        kill(_client_pid, SIGTERM);
    if (close_connection())
    {
        closelog();
        exit(EXIT_SUCCESS);
    }
    syslog(LOG_ERR, "host: couldn't close connection.");
    closelog();
    exit(EXIT_FAILURE);
}

void host_t::signal_handler(int sig, siginfo_t *info, void *context)
{
    host_t &instance = get_instance();
    switch (sig) 
    {
    case SIGUSR1:
        syslog(LOG_NOTICE, "host: SIGUSR1 signal catched.");
        if (instance._client_pid == info->si_pid)
            instance.attach_client(0);
        else 
        {
            syslog(LOG_NOTICE, "host: attaching client with pid %d.", info->si_pid);
            if (instance._is_client_attached)
                kill(instance._client_pid, SIGTERM);
            instance.attach_client(info->si_pid);
        }
        break;
    case SIGTERM:
        syslog(LOG_NOTICE, "host: terminate signal catched.");
        instance.terminate();
        break;
    }
}

bool host_t::open_connection()
{
    _host_sem = sem_open(SEM_HOST_NAME, O_CREAT, 0666, 0);
    if (_host_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "host: couldn't open host semaphore.");
        return false;
    }
    _client_sem = sem_open(SEM_CLIENT_NAME, O_CREAT, 0666, 0);
    if (_client_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "host: couldn't open client semaphore.");
        sem_unlink(SEM_CLIENT_NAME);
        return false;
    }
    if (!_connection.conn_open(getpid(), true))
    {
        syslog(LOG_ERR, "host: couldn't open connection.");
        sem_unlink(SEM_HOST_NAME);
        sem_unlink(SEM_CLIENT_NAME);
        return false;
    }
    return true;
}

bool host_t::close_connection()
{
    if (_host_sem != SEM_FAILED)
    {
        _host_sem = SEM_FAILED;
        sem_unlink(SEM_HOST_NAME);
    }
    if (_client_sem != SEM_FAILED)
    {
        _client_sem = SEM_FAILED;
        sem_unlink(SEM_CLIENT_NAME);
    }
    return _connection.conn_close();
}

bool host_t::read_date(message_t &msg)
{
    int tmp;
    std::string date, s;
    std::vector<uint32_t> date_vector;
    std::cin.clear();
    std::cin >> date;
    
    if (date.length() == 0)
        return false;
    
    std::istringstream date_stream(date);  
    while (std::getline(date_stream, s, '.')) 
    {
        try
        {
            tmp = (uint32_t)std::stoul(s);
            date_vector.push_back(tmp);
        }
        catch (std::exception &e)
        {
            syslog(LOG_ERR, "host: couldn't get int from date.");
            return false;
        }
    }

    if (!_is_client_attached)
        return false;

    if (date_vector.size() != 3 
        || date_vector[0] == 0 || date_vector[0] > 31 
        || date_vector[1] == 0 || date_vector[1] > 12)
    {
        std::cout << "error: incorrect date." << std::endl;
        syslog(LOG_ERR, "host: incorrect date.");
        return false;
    }

    msg.day = date_vector[0];
    msg.mon = date_vector[1];
    msg.year = date_vector[2];

    return true;   
}

void host_t::run()
{
    struct timespec ts;
    message_t msg;
    int s;
    while (true)
    {
        if (!_is_client_attached)
            sleep(1);
        else
        {
            if (_is_client_attached && !read_date(msg))
                continue;
            if (!_is_client_attached)
                continue;
            #ifndef host_sock
            _connection.conn_send(&msg, MESSAGE_SIZE);
            #else
            if (!_connection.conn_send(&msg, MESSAGE_SIZE) && errno == EPIPE)
            {
                if (!_connection.conn_open(getpid(), true))
                    terminate();
                _connection.conn_send(&msg, MESSAGE_SIZE);
            }
            #endif
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            sem_post(_client_sem);
            while ((s = sem_timedwait(_host_sem, &ts)) == -1 && errno == EINTR)
                continue;
            if (s == -1)
            {
                if (_is_client_attached)
                    kill(_client_pid, SIGTERM);
                attach_client(0);
            }
            else if (_connection.conn_recv(&msg, MESSAGE_SIZE))
            {
                std::cout << "client sent temperature " << msg.temp << std::endl;
                syslog(LOG_NOTICE, "host: client sent temperature %d.", msg.temp);
            }
        }   
    }
}

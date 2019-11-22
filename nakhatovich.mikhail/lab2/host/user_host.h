#ifndef USER_HOST_H
#define USER_HOST_H

#include <semaphore.h>
#include <signal.h>

#include "connect.h"
#include "message.h"

class host_t
{
public:
    static host_t & get_instance();

    bool open_connection();
    void run();

private:
    host_t(host_t const&) = delete;
    host_t& operator=(host_t const&) = delete;

    host_t();
    ~host_t();

    bool read_date(message_t &msg);
    bool close_connection();
    void attach_client(int client_pid);
    
    static void signal_handler(int sig, siginfo_t *info, void *context);

    int _client_pid;
    bool _is_client_attached;
    sem_t *_host_sem, *_client_sem;
    conn_t _connection;
};

#endif // USER_HOST_H

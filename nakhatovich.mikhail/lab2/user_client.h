#ifndef USER_CLIENT_H
#define USER_CLIENT_H

#include <semaphore.h>
#include <signal.h>

#include "connect.h"
#include "message.h"

class client_t
{
public:
    static client_t & get_instance(int host_pid);

    bool open_connection();
    void run();

private:
    client_t() = delete;
    client_t(client_t const&) = delete;
    client_t& operator=(client_t const&) = delete;

    client_t(int host_pid);
    ~client_t();

    void calculate_temperature(message_t &msg);
    bool close_connection();
   
    static void signal_handler(int sig, siginfo_t *info, void *context);

    int _host_pid;
    sem_t *_host_sem, *_client_sem;
    conn_t _connection;
};


#endif // USER_CLIENT_H

#ifndef CLIENT_H
#define CLIENT_H

#include <conn.h>
#include <semaphore.h>
#include <message.h>
#include <csignal>


class client_t 
{
public:
    static client_t& get_instance(int host_pid);
    bool is_connected;
    void run();
    int connect();
    int disconnect();
    int send_temperature(message_t *msg);
    message_t* receive_date();

    static void signal_handler(int signum, siginfo_t *info, void *context);
    
private:
    client_t(int host_pid);
    client_t();
    client_t(client_t&) = delete;
    ~client_t();

    void terminate();
    int connect_with_host();
    
    int host_pid;
    sem_t *semaphore_client, *semaphore_host;
    conn_t connection;
};


#endif
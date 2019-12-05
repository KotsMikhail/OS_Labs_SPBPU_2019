#ifndef HOST_H
#define HOST_H

#include <conn.h>
#include <semaphore.h>
#include <message.h>
#include <csignal>

class host_t 
{
public:
    static host_t& get_instance();
    void run();
    int connect();
    int disconnect();
    int send_date();
    int receive_temperature();

    static void signal_handler(int signum, siginfo_t *info, void *context);
    
private:
    host_t();
    host_t(host_t&) = delete;
    ~host_t();

    void terminate();
    message_t* read_date();
    
    int client_pid;
    sem_t *semaphore_host, *semaphore_client;
    message_t *curr_date;
    conn_t connection;
};

#endif
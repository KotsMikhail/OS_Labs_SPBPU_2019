//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_HOST_IMPL_H
#define LAB2_HOST_IMPL_H
#include <semaphore.h>
#include <csignal>
#include <conn.h>

struct ClientData
{
    int pid;
    bool attached;

    ClientData(int pid_) : pid(pid_), attached(pid_ != 0){}
};

class Host
{
public:
    static Host& getInstance();
    void run();
    bool openConnection();
private:
    sem_t* client_semaphore{};
    sem_t* host_semaphore{};
    Conn connection;
    ClientData client_data;
    bool readDate(Message &msg);
    static void signalHandler(int sig, siginfo_t* info, void* ptr);
    static void close(int exit_code = 0);

    Host& operator=(const Host&h) = delete;
    Host(const Host& h) = delete;
    Host();
};

#endif //LAB2_HOST_IMPL_H

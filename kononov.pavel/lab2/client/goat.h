#ifndef OS_LABS_SPBPU_2019_GOAT_H
#define OS_LABS_SPBPU_2019_GOAT_H

#include <conn.h>
#include <semaphore.h>
#include <message.h>
#include <csignal>


class Goat{
public:
    void Start();

    bool OpenConnection();

    static Goat &GetInstance();

    void SetHostPid(int pid);

    Goat(Goat &) = delete;

    Goat(const Goat &) = delete;

    Goat &operator=(const Goat &) = delete;

private:
    int host_pid;
    static const int RAND_LIMIT_ALIVE = 100;
    static const int RAND_LIMIT_DEAD = 50;

    void Terminate(int signum);

    Goat();

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);

    int id;
    Conn connection;
    sem_t *semaphore_host;
    sem_t *semaphore_client;
};

#endif //OS_LABS_SPBPU_2019_GOAT_H

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
    int m_host_pid;
    static const int RAND_LIMIT_ALIVE = 100;
    static const int RAND_LIMIT_DEAD = 50;

    void Terminate(int signum);

    Goat();

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);

    int m_id;
    Conn m_connection;
    sem_t *m_semaphore_host;
    sem_t *m_semaphore_client;
};

#endif //OS_LABS_SPBPU_2019_GOAT_H
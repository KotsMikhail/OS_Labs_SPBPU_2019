#ifndef OS_LABS_SPBPU_2019_WOLF_H
#define OS_LABS_SPBPU_2019_WOLF_H

#include <csignal>
#include "client_info.h"
#include "identifier.h"
#include <map>
#include <unistd.h>
#include <atomic>


class Wolf {
public:
    void Start();

    bool OpenConnection();

    static Wolf &GetInstance();

    Wolf(const Wolf &) = delete;

    Wolf &operator=(const Wolf &) = delete;

    ~Wolf();

    void SetClientsNum(int n);

private:
    ClientInfo *client_info;
    pthread_t *threads;
    pthread_attr_t *attr;
    int _clientsNum;
    Identifier identifier;

    int curr_num;
    const int RAND_LIMIT = 100;

    void Terminate(int signum);

    Wolf();

    static Message Step(Message &msg, ClientInfo &info);

    void GetNumber();

    static void *ThreadRun(void *pthrdData);

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);


    std::atomic<int> finished;
    std::atomic<int> step;
    pthread_cond_t cond;
    pthread_mutex_t mutx;

};

#endif //OS_LABS_SPBPU_2019_WOLF_H

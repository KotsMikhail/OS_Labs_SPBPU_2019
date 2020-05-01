#ifndef OS_LABS_SPBPU_2019_WOLF_H
#define OS_LABS_SPBPU_2019_WOLF_H

#include <csignal>
#include "client_info.h"
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
    ClientInfo *m_client_info;
    pthread_t *m_threads;
    pthread_attr_t *m_attr;
    int m_clientsNum;

    int m_curr_num;
    const int RAND_LIMIT = 100;

    void Terminate(int signum);

    Wolf();

    Message Step(Message &msg, ClientInfo &info);

    int GetClientId();

    void GetNumber();

    static void *ThreadRun(void *pthrdData);

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);


    std::atomic<int> m_finished;
    std::atomic<int> m_step;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutx;

};

#endif //OS_LABS_SPBPU_2019_WOLF_H

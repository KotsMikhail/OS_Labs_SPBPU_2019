#ifndef OS_LABS_SPBPU_2019_WOLF_H
#define OS_LABS_SPBPU_2019_WOLF_H

#include <csignal>
#include <member.h>
#include "client_info.h"
#include "identifier.h"
#include <map>
#include <unistd.h>
#include <atomic>


class Wolf: public Member
{
public:
    void Start();
    bool OpenConnection();

    static Wolf& GetInstance(int n);
    Wolf(Wolf&) = delete;
    Wolf(const Wolf&) = delete;
    Wolf& operator=(const Wolf&) = delete;
private:
    static ClientInfo* client_info;
    static pthread_t* threads;
    static pthread_attr_t* attr;
    static int _clientsNum;
    static Identifier identifier;

    static int curr_num;
    static const int RAND_LIMIT = 100;

    void Terminate(int signum);

    Wolf(int n);
    static Message Step(Message& msg, ClientInfo &info);
    void GetNumber();
    static void *ThreadRun(void *pthrdData);
    static void SignalHandler(int signum, siginfo_t* info, void *ptr);


    static std::atomic<int> finished;
    static std::atomic<int> step;
    static pthread_cond_t cond;
    static pthread_mutex_t mutx;

};

#endif //OS_LABS_SPBPU_2019_WOLF_H

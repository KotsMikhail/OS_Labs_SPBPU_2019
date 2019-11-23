//
// Created by dmitrii on 23.11.2019.
//

#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <semaphore.h>
#include <conn.h>
class Client
{
public:
    static Client& getInstance(int host_pid);
    void run();
    bool openConnection();
private:
    Conn connection;
    int skiped_messages;
    sem_t* semaphore{};
    int host_pid;
    bool need_post;

    explicit Client(int host_pid);
    void close(int signum);
    bool checkTimeout(Memory& msg);
    static int getRandTemp(const Date& date);
    static void signalHandler(int signum);
};


#endif //LAB2_CLIENT_H

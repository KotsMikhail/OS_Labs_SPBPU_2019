//
// Created by dmitrii on 19.11.2019.
//

#ifndef LAB2_CLIENT_DATA_H
#define LAB2_CLIENT_DATA_H

struct ClientData
{
    int pid;
    bool attached;
    int skiped_messages;

    ClientData(int pid_) : pid(pid_), attached(pid_ != 0), skiped_messages(0) {}
};

#endif //LAB2_CLIENT_DATA_H

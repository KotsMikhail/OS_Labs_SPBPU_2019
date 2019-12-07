//
// Created by misha on 23.11.2019.
//

#ifndef LAB_2_SEVER__CLIENT_INFO_H
#define LAB_2_SEVER__CLIENT_INFO_H

struct ClientInfo {
    int pid;
    bool attached;

    explicit ClientInfo(int pid) : pid(pid), attached(pid != 0) {
    }
};

#endif //LAB_2_SEVER__CLIENT_INFO_H

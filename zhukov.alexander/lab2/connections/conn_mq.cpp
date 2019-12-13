#include "conn.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>


void conn::Open(size_t id) {
    fd = (int*) malloc(sizeof(int));
    int mqperm = 0666;
    struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(message), 0, {0}});
    std::string nameSTR = "/lab1queue" + std::to_string(id);
    mq_unlink(nameSTR.c_str());
    *fd = mq_open(nameSTR.c_str(), O_RDWR | O_CREAT, mqperm, &attr);
    name = nameSTR.c_str();
}

void conn::Read(void *buf, size_t count) {
    mq_receive(*fd, (char *) buf, count, nullptr);
}

void conn::Write(void *buf, size_t count) {
    if (count <= sizeof(message)) {
        mq_send(*fd, (char *) buf, count, 0);
    }
}

void conn::Close() {
    mq_unlink(name);
    mq_close(*fd);
}

#include "conn.h"
#include "Message.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>

bool conn::Open(size_t id) {
    struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(message), 0, {0}});
    name = "/lab2_queue" + std::to_string(id);
    mq_unlink(name.c_str());
    fd = new int;
    if (fd != nullptr) {
        if ((*fd = mq_open(name.c_str(), O_RDWR | O_CREAT, 0666, &attr)) != -1) {
            return true;
        }
    }
    std::cout << "Error on opening connection for client " << id << std::endl;
    return false;
}

bool conn::Read(void *buf, size_t count) {
    if (mq_receive(*fd, (char *) buf, count, nullptr) != -1){
        return true;
    }
    std::cout << "Error on reading message" << std::endl;
    return false;
}

bool conn::Write(void *buf, size_t count) {
    if (count <= sizeof(message)) {
        if (mq_send(*fd, (char *) buf, count, 0) != -1){
            return true;
        }
    }
    std::cout << "Error on writing message" << std::endl;
    return false;
}

void conn::Close() {
    mq_unlink(name.c_str());
    mq_close(*fd);
}

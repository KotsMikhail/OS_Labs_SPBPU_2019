#include "conn.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>


bool conn::Open(size_t id) {
    fd = new int;
    if (fd == nullptr) {
        return false;
    }
    int mode = 0666;
    struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(message), 0, {0}});
    std::string nameSTR = "/lab1queue" + std::to_string(id);
    mq_unlink(nameSTR.c_str());
    if ((*fd = mq_open(nameSTR.c_str(), O_CREAT | O_RDWR, mode, &attr)) == -1) {
        delete(fd);
        std::cout << "ERROR: creating failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    name = nameSTR.c_str();
    return true;

}

bool conn::Read(void *buf, size_t count) {
    if (mq_receive(*fd, (char *) buf, count, nullptr) == -1) {
        std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool conn::Write(void *buf, size_t count) {
    if (mq_send(*fd, (char *) buf, count, 0) == -1) {
        std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void conn::Close() {
    mq_unlink(name);
    mq_close(*fd);
    delete(fd);
}

#include <conn.h>
#include <message.h>
#include "../utils/utils.h"

#include <iostream>
#include <sys/shm.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <cerrno>


Conn::Conn () {
}

Conn::~Conn () {
}

bool Conn::Open(size_t id, bool create) {
    _owner = create;
    _name = "/LAB2_QUEUE";
    _id = id;
    int mq_flg = O_RDWR;
    if (_owner) {
        std::cout << "Creating connection with id: " << id << std::endl;
        mq_flg |= O_CREAT;
        struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(Message), 0, {0}});
        _desc = mq_open(GetName(_name.c_str(), _id).c_str(), mq_flg, 0666, &attr);
    } else {
        std::cout << "Getting connection with id: " << id << std::endl;
        _desc = mq_open(GetName(_name.c_str(), _id).c_str(), mq_flg);
    }
    if (_desc == -1) {
        std::cout << "ERROR: mq_open failed, errno: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Read(void* buf, size_t count) {
    if (mq_receive(_desc, (char *) buf, count, nullptr) == -1) {
        std::cout << "ERROR: mq_recieve failed, errno: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Write(void* buf, size_t count) {
    if (mq_send(_desc, (char *) buf, count, 0) == -1) {
        std::cout << "ERROR: mq_send failed, errno: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Close() {
    if (mq_close(_desc) == 0) {
        if (!_owner || (mq_unlink(GetName(_name.c_str(), _id).c_str()) == 0)) {
            return true;
        }
    }
    return false;
}

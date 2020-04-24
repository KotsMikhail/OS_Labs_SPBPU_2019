#include <conn.h>

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <message.h>
#include <cerrno>
#include "../utils/utils.h"

Conn::Conn () {
}

Conn::~Conn () {
}

bool Conn::Open(size_t id, bool create) {
    _owner = create;
    _name = "/tmp/lab2_fifo";
    _id = id;

    if (_owner && mkfifo(GetName(_name.c_str(), _id).c_str(), 0777) == -1) {
        std::cout << "ERROR: mkfifo failed: " << strerror(errno) << std::endl;
        return false;
    } else {
        _desc = open(GetName(_name.c_str(), _id).c_str(), O_RDWR);
        if (_desc == -1) {
            std::cout << "ERROR: open failed: " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

bool Conn::Read(void* buf, size_t count) {
    if (read(_desc, buf, count) == -1) {
        std::cout << "ERROR: reading failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Write(void* buf, size_t count) {
    if (write(_desc, buf, count) == -1) {
        std::cout << "ERROR: writing failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Close() {
    if (close(_desc) < 0 || (_owner && remove(GetName(_name.c_str(), _id).c_str()) < 0)) {
        std::cout << "ERROR: close failed: " << strerror(errno) << std::endl;
        std::cout << GetName(_name.c_str(), _id).c_str() << std::endl;
        return false;
    }
    return true;
}

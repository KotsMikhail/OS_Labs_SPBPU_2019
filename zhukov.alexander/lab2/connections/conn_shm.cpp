#include "conn.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>


off_t length = 1024;

bool conn::Read(void *buf, size_t count) {
    if (memcpy(buf, fd, count) == nullptr) {
        std::cout << "ERROR: reading failed" << std::endl;
        return false;
    }
    return true;

}

bool conn::Write(void *buf, size_t count) {
    if (memcpy(fd, buf, count) == nullptr) {
        std::cout << "ERROR: writing failed" << std::endl;
        return false;
    }
    return true;

}

bool conn::Open(size_t id) {
    std::string nameSTR = "shm_file" + std::to_string(id);
    int shm;
    if ((shm = shm_open(nameSTR.c_str(), O_CREAT | O_RDWR, 0666)) == -1) {
        std::cout << "ERROR: shm_open failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    ftruncate(shm, length);
    fd = (int *) mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (fd == (int *) -1) {
        std::cout << "ERROR: mmap failed with error = " << strerror(errno) << std::endl;
        return false;
    }
    name = nameSTR.c_str();
    return true;
}

void conn::Close() {
    munmap(fd, length);
    shm_unlink(name);
}
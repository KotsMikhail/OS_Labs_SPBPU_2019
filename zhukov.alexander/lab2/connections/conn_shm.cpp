#include "conn.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <map>


off_t length = 1024;

void conn::Read(void *buf, size_t count) {
    memcpy(buf, fd, count);
}

void conn::Write(void *buf, size_t count) {
    memcpy(fd, buf, count);
}

void conn::Open(size_t id) {
    std::string nameSTR = "shm_file" + std::to_string(id);
    int shm = shm_open(nameSTR.c_str(), O_RDWR | O_CREAT, 0666);
    ftruncate(shm, length);
    fd = (int *) mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    name = nameSTR.c_str();
}

void conn::Close() {
    munmap(fd, length);
    shm_unlink(name);
}
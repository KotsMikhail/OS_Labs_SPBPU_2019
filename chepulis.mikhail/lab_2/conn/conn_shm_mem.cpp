//
// Created by misha on 25.11.2019.
//


#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include "../interfaces/Conn.h"
#include "../host/my_functions.h"
#include <sys/mman.h>
#include <unistd.h>
#include <syslog.h>
#include "../interfaces/message.h"

static std::string mem_filename = "/lab2_shm_mem";

bool Conn::Open(size_t id, bool create) {
    owner = create;
    int mode = 0777;
    filename = mem_filename + "_" + toString(id);
    int shm;
    if (create) {
        //std::cout << "Creating connection with id = " << id << ", file = " << filename << std::endl;
        syslog(LOG_NOTICE, "Creating connection with id = %i, file = %s", (int)id, filename.c_str());
        if ((shm = shm_open(filename.c_str(), O_CREAT | O_RDWR, mode)) == -1)
        {
            //std::cout << "ERROR: shm_open failed, error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: shm_open failed, error = %s", strerror(errno));
            return false;
        }
        ftruncate(shm, sizeof(Message));
    } else {
        //std::cout << "Getting connection with id = " << id << ", file = " << filename << std::endl;
        syslog(LOG_NOTICE, "Getting connection with id = %i, file = %s", (int)id, filename.c_str());
        if ((shm = shm_open(filename.c_str(), O_RDWR, mode)) == -1)
        {
            //std::cout << "ERROR: shm_open failed, error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: shm_open failed, error = %s", strerror(errno));
            return false;
        }
    }
    fd = (int*) mmap(0, sizeof(Message), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    if ( fd == (int*)-1 ) {
        perror("mmap");
        //std::cout << "ERROR: mmap failed, error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: mmap failed, error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Read(void *buf, size_t count) {
    if (count > sizeof(Message)) {
        //std::cout << "ERROR: Wrong size" << std::endl;
        syslog(LOG_ERR, "ERROR: Wrong size");
        return false;
    }
    memcpy(buf, fd, count);
    return true;
}

bool Conn::Write(void *buf, size_t count) {
    if (count > sizeof(Message)) {
        //std::cout << "ERROR: Wrong size" << std::endl;
        syslog(LOG_ERR, "ERROR: Wrong size");
        return false;
    }
    memcpy(fd, buf, count);
    return true;
}

bool Conn::Close() {

    if (owner && (shm_unlink(filename.c_str()) == -1) ){
        return false;
    }
    return true;
}

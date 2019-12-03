//
// Created by misha on 24.11.2019.
//


#include <iostream>
#include <fcntl.h>
#include <mqueue.h>
#include <cstring>
#include "../interfaces/Conn.h"
#include "../host/my_functions.h"
#include "../interfaces/message.h"
#include <syslog.h>


static std::string mq_filename = "/lab2_mq";


bool Conn::Open(size_t id, bool create) {
    fd = (int*) malloc(sizeof(int));
    owner = create;
    filename = mq_filename + "_" + toString(id);
    int mode = 0666;
    if (create) {
        //std::cout << "Creating connection with id = " << id << ", file = " << filename << std::endl;
        syslog(LOG_NOTICE, "Creating connection with id = %i, file = %s", (int)id, filename.c_str());

        struct mq_attr attr = {0, 1, sizeof(Message), 0, {0}};
        if((*fd = mq_open(filename.c_str(), O_CREAT | O_RDWR, mode, &attr)) == -1)
        {
            //std::cout << "can`t create = " << id << ", file = " << filename << "\twith error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: can`t create connection with id = %i, file = %s, error = %s", (int)id, filename.c_str(), strerror(errno));
            return false;
        }

    } else {
        //std::cout << "Getting connection with id = " << id << ", file = " << filename << std::endl;
        syslog(LOG_NOTICE, "Getting connection with id = %i, file = %s", (int)id, filename.c_str());
        if((*fd = mq_open(filename.c_str(), O_RDWR)) == -1)
        {
            //std::cout << "can`t open = " << id << ", file = " << filename << "\twith error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: can`t open connection with id = %i, file = %s, error = %s", (int)id, filename.c_str(), strerror(errno));
            return false;
        }
    }
    return true;
}

bool Conn::Read(void *buf, size_t count) {
    if (count > sizeof(Message)) {
        //std::cout << "ERROR: Wrong size" << std::endl;
        syslog(LOG_ERR, "ERROR: Wrong size");
        return false;
    }

    if (mq_receive(*fd, (char *)buf, count, nullptr) == -1)
    {
        std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: reading failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Write(void *buf, size_t count) {
    if (count > sizeof(Message)) {
        //std::cout << "Wrong size" << std::endl;
        syslog(LOG_ERR, "ERROR: Wrong size");
        return false;
    }
    if (mq_send(*fd, (char *)buf, count, 0) == -1)
    {
        std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: writing failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Close() {
    if (!mq_close(*fd)) {

        free(fd);
        if (owner && mq_unlink(filename.c_str())) {
            return false;
        }
        return true;
    }
    free(fd);
    return true;
}

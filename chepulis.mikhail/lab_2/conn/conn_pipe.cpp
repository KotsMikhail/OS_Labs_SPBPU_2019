//
// Created by misha on 24.11.2019.
//


#include "../interfaces/Conn.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <syslog.h>

bool Conn::Open(size_t id, bool create) {

    owner = create;
    if (create){
        fd = (int*) malloc( 2 * sizeof(int));
        int tmp[2];
        //std::cout << "Creating connection with id = " << id  << std::endl;
        syslog(LOG_NOTICE, "Creating connection with id = %i", (int)id);
        if (pipe(tmp) == -1){
            perror("pipe");
            //std::cout << "ERROR: pipe failed, error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: pipe failed, error = %s", strerror(errno));
            return false;
        }
        fd[0] = tmp[0];
        fd[1] = tmp[1];
    }
    else{
        //std::cout << "Getting connection with id = " << id << std::endl;
        syslog(LOG_NOTICE, "Getting connection with id =  %i", (int)id);
    }

    return true;
}

bool Conn::Read(void *buf, size_t count) {
    if (read(fd[0], buf, count) <= 0) {
        //std::cout << "ERROR: reading failed with error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: reading failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}


bool Conn::Write(void *buf, size_t count) {
    if (write(fd[1], buf, count) == -1) {
        //std::cout << "ERROR: writing failed with error = " << strerror(errno) << std::endl;
        syslog(LOG_ERR, "ERROR: writing failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Close() {
    if (owner){
        if ((close(fd[0]) < 0) &&  (close(fd[1]) < 0)){
            free(fd);
            return false;
        }
        free(fd);
    }

    return true;
}

#include <iostream>
#include "client_info.h"
#include "../utils/utils.h"
#include <fcntl.h>

void ClientInfo::Delete() {
    if (semaphore_host == nullptr || semaphore_client == nullptr)
        return;

    if (sem_unlink(GetName(SEMAPHORE_HOST_NAME, id).c_str()) == -1 ||
        sem_unlink(GetName(SEMAPHORE_CLIENT_NAME, id).c_str()) == -1) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
    }
    if (!connection.Close()) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
    }
    semaphore_client = nullptr;
    semaphore_host = nullptr;
}

bool ClientInfo::OpenConnection(int i) {
    bool res = false;
    id = i;
    sem_unlink(GetName(SEMAPHORE_HOST_NAME, id).c_str());
    sem_unlink(GetName(SEMAPHORE_CLIENT_NAME, id).c_str());
    semaphore_host = sem_open(GetName(SEMAPHORE_HOST_NAME, i).c_str(), O_CREAT, 0666, 0);
    semaphore_client = sem_open(GetName(SEMAPHORE_CLIENT_NAME, i).c_str(), O_CREAT, 0666, 0);

    if (connection.Open(i, true)) {
        if (semaphore_host == SEM_FAILED ||
            semaphore_client == SEM_FAILED) {
            std::cout << "ERROR: sem_open failed with error: " << strerror(errno) << std::endl;
            res = false;
        } else {
            res = true;
        }
    }
    return res;
}

void ClientInfo::Dettach() {
    attached = false;
}

void ClientInfo::Attach(int pid) {
    this->pid = pid;
    this->attached = true;
}

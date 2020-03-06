#include <iostream>
#include "client_info.h"
#include "../utils/utils.h"
#include <fcntl.h>

void ClientInfo::Delete()
{
	if (pid == 0)
		return;

	if (sem_unlink(GetName(SEMAPHORE_HOST_NAME, id).c_str()) == -1 || 
            sem_unlink(GetName(SEMAPHORE_CLIENT_NAME, id).c_str()) == -1)
        {
		std::cout << "Failed: " << strerror(errno) << std::endl;
        }
	if (!connection.Close())
	{
		std::cout << "Failed: " << strerror(errno) << std::endl;
	}
	pid = 0;
}


bool ClientInfo::OpenConnection(int i)
{
	bool res;
	id = i;
        if (connection.Open(i, true))
        {
            semaphore_host = sem_open(GetName(SEMAPHORE_HOST_NAME, i).c_str(), O_CREAT, 0666, 0);
            semaphore_client = sem_open(GetName(SEMAPHORE_CLIENT_NAME, i).c_str(), O_CREAT, 0666, 0);
            std:: cout << "Connection Opened sem_host:" << semaphore_host << " sem_client: " << semaphore_client << std::endl; // debug
            if (semaphore_host == SEM_FAILED || 
                semaphore_client == SEM_FAILED)
            {
                std::cout << "ERROR: sem_open failed with error: " << strerror(errno) << std::endl;
                res = false;
            }
            else
            {
                res = true;
            }
        }
        return res;
}


void ClientInfo::Dettach()
{
	attached = false;
	pid = 0;
}

void ClientInfo::Attach(int pid)
{
	this->pid = pid;
	this->attached = true;
}

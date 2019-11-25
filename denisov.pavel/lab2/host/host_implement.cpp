#include <iostream>
#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include <message.h>

#include "host.h"


static void HostSignalHandler (int signalNum, siginfo_t* info, void* ptr)
{
    static Host& host = Host::GetInstance();
    switch (signalNum) {
        // synchronize with client signal
        case SIGUSR1:
            if (host.IsClientAttached()) {
                std::cout << "The client has already attached. Is can be only one client." << std::endl;
            } else {
                std::cout << "Attaching new client with pid: " << info->si_pid << std::endl;
                host.AttachNewClient(info->si_pid);
            }
            break;

        // client failed signal
        case SIGUSR2:
            std::cout << "Client failed." << std::endl;
            if (host.GetClientPid() == info->si_pid) {
                host.AttachNewClient(0);
            }
            break;

        case SIGTERM:
            if (host.IsClientAttached()) {
                host.DeattachClient();
            }
            host.Terminate();
            break;

        case SIGINT:
            host.Terminate();
            break;
    }
}


Host& Host::GetInstance ()
{
    static Host inst;
    return inst;
}


Host::Host ()
: curClientInfo(0)
{
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_sigaction = HostSignalHandler;
    sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGUSR2, &sigact, NULL);
    sigaction(SIGUSR1, &sigact, NULL);
}


bool Host::IsClientAttached ()
{
    return curClientInfo.isAttached;
}


void Host::AttachNewClient (pid_t clientPid)
{
    curClientInfo = Client(clientPid);
}


void Host::DeattachClient ()
{
    kill(GetClientPid(), SIGTERM);
    AttachNewClient(0);
}


pid_t Host::GetClientPid ()
{
    return curClientInfo.pid;
}


bool Host::OpenConnection ()
{
    if ((semaphore_host = sem_open(HOST_SEM_NAME, O_CREAT, 0666, 0)) == SEM_FAILED) {
        std::cout << "[ERROR]: failed to create host semaphore, error: " << strerror(errno) << std::endl;
        return false;
    }

    if ((semaphore_client = sem_open(CLIENT_SEM_NAME, O_CREAT, 0666, 0)) == SEM_FAILED) {
        std::cout << "[ERROR]: failed to create client semaphore, error: " << strerror(errno) << std::endl;
        sem_unlink(HOST_SEM_NAME);
        return false;
    }

    std::cout << "Semaphores created. Host's pid: " << getpid() << std::endl;
    if (!conn.Open(getpid(), true)) {
        std::cout << "[ERROR]: Failed to open connection." << std::endl;
        sem_unlink(HOST_SEM_NAME);
        sem_unlink(CLIENT_SEM_NAME);
        return false;
    }

    return true;
}


void Host::SendFirstMessageToClient ()
{
    std::cout << "Input new host number: ";
    std::cin >> curNumber;
    Message msg(MSG_OWNER::HOST, MSG_STATUS::ALIVE, curNumber);
    conn.Write((void *) &msg);
}


void Host::Start ()
{
    struct timespec ts;

    // Waiting while client will be attached - signal SIGUSR1
    std::cout << "Wait client..." << std::endl;
    pause();
    SendFirstMessageToClient();

    Message msg;
    // Job loop
    while (true) {
        if (!IsClientAttached()) {
            // this 'sleep' guaranteed that client will send SIGUSR2 before 'pause()'
            sleep(1);

            // Waiting while client will be attached - signal SIGUSR1
            std::cout << "Wait client..." << std::endl;
            pause();
            SendFirstMessageToClient();
            continue;
        }

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += TIMEOUT;
        sem_post(semaphore_client);
        if (sem_timedwait(semaphore_host, &ts) == -1) {
            std::cout << "Client timeouted. Terminating client..." << std::endl;
            DeattachClient();
            continue;
        }

        if (conn.Read(&msg)) {
            std::cout << "------------HOST-----------" << std::endl;
            std::cout << "Host's number: " << curNumber << std::endl;
            std::cout << "Client's number: " << msg.number << std::endl;
            std::cout << "Client's status in begin of round: " << ((msg.status == MSG_STATUS::ALIVE) ? "ALIVE" : "DEAD") << std::endl;

            msg = CountClientStatus(msg);

            if (IsClientAttached()) {
                std::cout << "Client's status after round: " << ((msg.status == MSG_STATUS::ALIVE) ? "ALIVE" : "DEAD") << std::endl;
                conn.Write((void *) &msg);

                std::cout << "Input new number: ";
                std::cin >> curNumber;
            }

            std::cout << "------------HOST-----------" << std::endl;
        }
    }
}


Message Host::CountClientStatus (const Message& curClientMessage)
{
    Message hostMsg(MSG_OWNER::HOST, MSG_STATUS::ALIVE, 0);

    if ((curClientMessage.status == MSG_STATUS::ALIVE && abs(curClientMessage.number - curNumber) <= 70) ||
        (curClientMessage.status == MSG_STATUS::DEAD && abs(curClientMessage.number - curNumber) <= 20)) {
        curClientInfo.numOfFails = 0;
    } else {
        hostMsg.status = MSG_STATUS::DEAD;
        curClientInfo.numOfFails++;
        if (curClientInfo.numOfFails == 2) {
            std::cout << "Client fails 2 times. Deattaching client..." << std::endl;
            std::cout << "Client's status after round: DEAD" << std::endl;
            DeattachClient();

            hostMsg.status = MSG_STATUS::ALIVE;
        }
    }

    return hostMsg;
}


void Host::Terminate ()
{
    std::cout << "Host terminating..." << std::endl;
    if ((semaphore_host != SEM_FAILED && sem_unlink(HOST_SEM_NAME) == 0) && (semaphore_client != SEM_FAILED && sem_unlink(CLIENT_SEM_NAME) == 0) && conn.Close()) {
        exit(SIGTERM);
    }
    exit(errno);
}
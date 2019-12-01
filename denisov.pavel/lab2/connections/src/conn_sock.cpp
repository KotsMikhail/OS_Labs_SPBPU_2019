#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <new>
#include <unistd.h>

#include <conn.h>
#include <message.h>

const char * const SOCK_SERVER_PATH = "/tmp/lab2_socket_server";
const int MAXMSGSNUM = 1;


struct ConnSockArgs
{
    bool wasCreated;
    int listener;
    int sock;
};


Conn::Conn ()
{
    pmem = new (std::nothrow) ConnSockArgs();
}


Conn::~Conn ()
{
    delete (ConnSockArgs *)pmem;
}


bool Conn::Open (size_t id, bool create)
{
    ConnSockArgs *sockArgs = (ConnSockArgs *)pmem;
    if (sockArgs == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, SOCK_SERVER_PATH);

    isHost = create;
    if (sockArgs->wasCreated) {
        if (isHost) {
            sockArgs->sock = accept(sockArgs->listener, NULL, NULL);
            if (sockArgs->sock == -1) {
                std::cout << "[ERROR]: [HOST]: failed to accept the socket, error: " << strerror(errno) << std::endl;
                close(sockArgs->listener);
                unlink(SOCK_SERVER_PATH);
                return false;
            }

            std::cout << "[HOST]: Socket accepted." << std::endl;
        } else {
            if (connect(sockArgs->sock, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1) {
                std::cout << "[ERROR]: [CLIENT]: failed to connect the socket, error: " << strerror(errno) << std::endl;
                close(sockArgs->sock);
                return false;
            }

            std::cout << "[CLIENT]: Socket connected." << std::endl;
        }

        return true;
    }

    if (create) {
        sockArgs->sock = -1;
        // create listener socket
        std::cout << "[HOST]: Creating listener." << std::endl;
        sockArgs->listener = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockArgs->listener == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to create listener, error: " << strerror(errno) << std::endl;
            return false;
        }
        std::cout << "[HOST]: Listener created." << std::endl;

        // bind listener socket
        std::cout << "[HOST]: Binding listener." << std::endl;
        if (bind(sockArgs->listener, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) < 0) {
            std::cout << "[ERROR]: [HOST]: Failed to bind listener, error: " << strerror(errno) << std::endl;
            close(sockArgs->listener);
            return false;
        }
        std::cout << "[HOST]: Listener binded." << std::endl;

        std::cout << "[HOST]: Listen the listener." << std::endl;
        if (listen(sockArgs->listener, MAXMSGSNUM) == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to listen, error: " << strerror(errno) << std::endl;
            close(sockArgs->listener);
            unlink(SOCK_SERVER_PATH);
            return false;
        }
        std::cout << "[HOST]: Listen success." << std::endl;

        std::cout << "[HOST]: Accepting the listener." << std::endl;
        sockArgs->sock = accept(sockArgs->listener, NULL, NULL);
        if (sockArgs->sock == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to accept the listener, error: " << strerror(errno) << std::endl;
            close(sockArgs->listener);
            unlink(SOCK_SERVER_PATH);
            return false;
        }
        std::cout << "[HOST]: Accepting success." << std::endl;
    } else {
        sockArgs->listener = -1;
        // create client's socket
        std::cout << "[CLIENT]: Creating socket." << std::endl;
        sockArgs->sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockArgs->sock == -1) {
            std::cout << "[ERROR]: [CLIENT]: Failed to create socket, error: " << strerror(errno) << std::endl;
            return false;
        }
        std::cout << "[CLIENT]: Socket created." << std::endl;

        // connecting client's socket
        std::cout << "[CLIENT]: Connecting socket." << std::endl;
        if (connect(sockArgs->sock, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1) {
            std::cout << "[ERROR]: [CLIENT]: Failed to connect socket, error: " << strerror(errno) << std::endl;
            close(sockArgs->sock);
            return false;
        }
        std::cout << "[CLIENT]: Socket connected." << std::endl;
    }

    sockArgs->wasCreated = true;
    return true;
}


bool Conn::Close ()
{
    ConnSockArgs *sockArgs = (ConnSockArgs *)pmem;
    if (sockArgs == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    if (sockArgs->wasCreated || sockArgs->listener != -1) {
        if (sockArgs->sock != -1) {
            if (close(sockArgs->sock) == -1) {
                std::cout << "[ERROR]: Failed to close sock, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (sockArgs->listener != -1) {
            if (close(sockArgs->listener) == -1) {
                std::cout << "[ERROR]: Failed to close listener, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!isHost || (isHost && unlink(SOCK_SERVER_PATH) == 0)) {
            std::cout << "Connection closed." << std::endl;
            sockArgs->wasCreated = false;
            return true;
        }

        std::cout << "[ERROR]: Connection closing failed, error: " << strerror(errno) << std::endl;
    }

    return true;
}


bool Conn::Read (void *buf, size_t count)
{
    ConnSockArgs *sockArgs = (ConnSockArgs *)pmem;
    if (sockArgs == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    if (recv(sockArgs->sock, buf, count, 0) == -1) {
        std::cout << "[ERROR]: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write (void *buf, size_t count)
{
    ConnSockArgs *sockArgs = (ConnSockArgs *)pmem;
    if (sockArgs == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    if (send(sockArgs->sock, buf, count, MSG_NOSIGNAL) == -1) {
        std::cout << "[ERROR]: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

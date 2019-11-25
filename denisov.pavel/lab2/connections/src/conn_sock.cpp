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


Conn::Conn ()
{
    additionalArgs = new (std::nothrow) int[2]();
    // additionalArgs[0] - listener
    // additionalArgs[1] - sock

    wasCreated = false;
}


Conn::~Conn ()
{
    delete [] additionalArgs;
}


bool Conn::Open (size_t id, bool create)
{
    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, SOCK_SERVER_PATH);

    isHost = create;
    if (wasCreated) {
        if (isHost) {
            additionalArgs[1] = accept(additionalArgs[0], NULL, NULL);
            if (additionalArgs[1] == -1) {
                std::cout << "[ERROR]: [HOST]: failed to accept the socket, error: " << strerror(errno) << std::endl;
                close(additionalArgs[0]);
                unlink(SOCK_SERVER_PATH);
                return false;
            }

            std::cout << "[HOST]: Socket accepted." << std::endl;
        } else {
            if (connect(additionalArgs[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1) {
                std::cout << "[ERROR]: [CLIENT]: failed to connect the socket, error: " << strerror(errno) << std::endl;
                close(additionalArgs[1]);
                return false;
            }

            std::cout << "[CLIENT]: Socket connected." << std::endl;
        }

        return true;
    }

    if (create) {
        // create listener socket
        std::cout << "[HOST]: Creating listener." << std::endl;
        additionalArgs[0] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (additionalArgs[0] == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to create listener, error: " << strerror(errno) << std::endl;
            return false;
        }
        std::cout << "[HOST]: Listener created." << std::endl;

        // bind listener socket
        std::cout << "[HOST]: Binding listener." << std::endl;
        if (bind(additionalArgs[0], (struct sockaddr *)&saddr, SUN_LEN(&saddr)) < 0) {
            std::cout << "[ERROR]: [HOST]: Failed to bind listener, error: " << strerror(errno) << std::endl;
            close(additionalArgs[0]);
            return false;
        }
        std::cout << "[HOST]: Listener binded." << std::endl;

        std::cout << "[HOST]: Listen the listener." << std::endl;
        if (listen(additionalArgs[0], MAXMSGSNUM) == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to listen, error: " << strerror(errno) << std::endl;
            close(additionalArgs[0]);
            unlink(SOCK_SERVER_PATH);
            return false;
        }
        std::cout << "[HOST]: Listen success." << std::endl;

        std::cout << "[HOST]: Accepting the listener." << std::endl;
        additionalArgs[1] = accept(additionalArgs[0], NULL, NULL);
        if (additionalArgs[1] == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to accept the listener, error: " << strerror(errno) << std::endl;
            close(additionalArgs[0]);
            unlink(SOCK_SERVER_PATH);
            return false;
        }
        std::cout << "[HOST]: Accepting success." << std::endl;
    } else {
        additionalArgs[0] = -1;
        // create client's socket
        std::cout << "[CLIENT]: Creating socket." << std::endl;
        additionalArgs[1] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (additionalArgs[1] == -1) {
            std::cout << "[ERROR]: [CLIENT]: Failed to create socket, error: " << strerror(errno) << std::endl;
            return false;
        }
        std::cout << "[CLIENT]: Socket created." << std::endl;

        // connecting client's socket
        std::cout << "[CLIENT]: Connecting socket." << std::endl;
        if (connect(additionalArgs[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1) {
            std::cout << "[ERROR]: [CLIENT]: Failed to connect socket, error: " << strerror(errno) << std::endl;
            close(additionalArgs[1]);
            return false;
        }
        std::cout << "[CLIENT]: Socket connected." << std::endl;
    }

    wasCreated = true;
    return true;
}


bool Conn::Close ()
{
    if (wasCreated && close(additionalArgs[1]) == 0) {
        if (additionalArgs[0] != -1) {
            if (close(additionalArgs[0]) == -1) {
                std::cout << "[ERROR]: Failed to close listener, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!isHost || (isHost && unlink(SOCK_SERVER_PATH) == 0)) {
            std::cout << "Connection closed." << std::endl;
            wasCreated = false;
            return true;
        }

        std::cout << "[ERROR]: Connection closing failed, error: " << strerror(errno) << std::endl;
    }

    return true;
}


bool Conn::Read (void *buf, size_t count)
{
    if (recv(additionalArgs[1], buf, count, 0) == -1) {
        std::cout << "[ERROR]: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write (void *buf, size_t count)
{
    if (send(additionalArgs[1], buf, count, MSG_NOSIGNAL) == -1) {
        std::cout << "[ERROR]: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

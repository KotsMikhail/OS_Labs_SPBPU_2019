#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <new>
#include <unistd.h>

#include <conn.h>
#include "../utils/utils.h"

const char * const SOCK_SERVER_PATH = "/tmp/lab2_socket_server";
const int MAXMSGSNUM = 1;


class SocketInfo {
public:
    bool created;
    int listener;
    int sock;
};

Conn::Conn () {
    m_pmem = new(std::nothrow) SocketInfo();
}

Conn::~Conn () {
    delete (SocketInfo *) m_pmem;
}

bool Conn::Open (size_t id, bool create) {
    SocketInfo *pInfo = (SocketInfo *) m_pmem;
    if (pInfo == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    m_id = id;

    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, GetName(SOCK_SERVER_PATH, m_id).c_str());

    m_owner = create;
    if (pInfo->created) {
        if (m_owner) {
            pInfo->sock = accept(pInfo->listener, NULL, NULL);
            if (pInfo->sock == -1) {
                std::cout << "[ERROR]: [HOST]: failed to accept the socket, error: " << strerror(errno) << std::endl;
                close(pInfo->listener);
                unlink(GetName(SOCK_SERVER_PATH, m_id).c_str());
                return false;
            }

        } else {
            if (connect(pInfo->sock, (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
                std::cout << "[ERROR]: [CLIENT]: failed to connect the socket, error: " << strerror(errno) << std::endl;
                close(pInfo->sock);
                return false;
            }
        }

        return true;
    }

    if (create) {
        pInfo->sock = -1;
        pInfo->listener = socket(AF_UNIX, SOCK_STREAM, 0);
        if (pInfo->listener == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to create listener, error: " << strerror(errno) << std::endl;
            return false;
        }

        if (bind(pInfo->listener, (struct sockaddr *) &saddr, SUN_LEN(&saddr)) < 0) {
            std::cout << "[ERROR]: [HOST]: Failed to bind listener, error: " << strerror(errno) << std::endl;
            close(pInfo->listener);
            return false;
        }

        if (listen(pInfo->listener, MAXMSGSNUM) == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to listen, error: " << strerror(errno) << std::endl;
            close(pInfo->listener);
            unlink(GetName(SOCK_SERVER_PATH, m_id).c_str());
            return false;
        }

        pInfo->sock = accept(pInfo->listener, NULL, NULL);
        if (pInfo->sock == -1) {
            std::cout << "[ERROR]: [HOST]: Failed to accept the listener, error: " << strerror(errno) << std::endl;
            close(pInfo->listener);
            unlink(GetName(SOCK_SERVER_PATH, m_id).c_str());
            return false;
        }
    } else {
        pInfo->listener = -1;
        pInfo->sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (pInfo->sock == -1) {
            std::cout << "[ERROR]: [CLIENT]: Failed to create socket, error: " << strerror(errno) << std::endl;
            return false;
        }

        if (connect(pInfo->sock, (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
            std::cout << "[ERROR]: [CLIENT]: Failed to connect socket, error: " << strerror(errno) << std::endl;
            close(pInfo->sock);
            return false;
        }
    }

    pInfo->created = true;
    return true;
}

bool Conn::Close () {
    SocketInfo *pInfo = (SocketInfo *) m_pmem;
    if (pInfo == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    if (pInfo->created || pInfo->listener != -1) {
        if (pInfo->sock != -1) {
            if (close(pInfo->sock) == -1) {
                std::cout << "[ERROR]: Failed to close sock, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (pInfo->listener != -1) {
            if (close(pInfo->listener) == -1) {
                std::cout << "[ERROR]: Failed to close listener, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!m_owner || (m_owner && unlink(GetName(SOCK_SERVER_PATH, m_id).c_str()) == 0)) {
            std::cout << "Connection closed." << std::endl;
            pInfo->created = false;
            return true;
        }

        std::cout << "[ERROR]: Connection closing failed, error: " << strerror(errno) << std::endl;
    }

    return true;
}

bool Conn::Read (void *buf, size_t count) {
    SocketInfo *pInfo = (SocketInfo *) m_pmem;
    if (pInfo == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    if (recv(pInfo->sock, buf, count, 0) == -1) {
        std::cout << "[ERROR]: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Write (void *buf, size_t count) {
    SocketInfo *pInfo = (SocketInfo *) m_pmem;
    if (pInfo == NULL) {
        std::cout << "[ERROR]: Socket connection's arguments invalid." << std::endl;
        return false;
    }

    if (send(pInfo->sock, buf, count, MSG_NOSIGNAL) == -1) {
        std::cout << "[ERROR]: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

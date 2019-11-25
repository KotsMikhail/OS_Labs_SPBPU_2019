//
// Created by dmitrii on 23.11.2019.
//

#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>
#include <conn.h>
#include <message.h>
#include <unistd.h>

struct SockInfo
{
    int listener;
    int sock;
};

Conn::Conn() : is_open(false)
{
    channel_name = "/tmp/sock_server";
    desc_ = new (std::nothrow) SockInfo();
}

Conn::~Conn()
{
    delete (SockInfo*)desc_;
}

void printErrMsg(const std::string& who, const std::string& msg)
{
    std::cout << "ERROR from " << who << ": " << msg << ", error is: "<< strerror(errno) << std::endl;
}

bool Conn::Open(size_t id, bool create)
{
    SockInfo* sockInfo = (SockInfo*)desc_;

    if (sockInfo == nullptr)
        return false;

    std::cout << "start open socket with address: " << channel_name << std::endl;
    struct sockaddr_un saddr{};
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, channel_name);

    is_host = create;
    if (is_open)
    {
        if (is_host)
        {
            sockInfo->sock = accept(sockInfo->listener, NULL, NULL);
            if (sockInfo->sock == -1)
            {
                printErrMsg("host", "fail to accept the socket");
                close(sockInfo->listener);
                unlink(channel_name);
                return false;
            }

        }
        else
        {
            if (connect(sockInfo->sock, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
            {
                printErrMsg("client", "fail to connect to socket");
                close(sockInfo->sock);
                return false;
            }
        }

        return true;
    }

    if (is_host)
    {
        if (unlink(channel_name) == -1)
            std::cout << "WARNING: can't unlink " << channel_name << std::endl;

        // create listener socket
        std::cout << "create listener starting" << std::endl;
        sockInfo->listener = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockInfo->listener == -1) {
            printErrMsg("host", "fail to create listener");
            return false;
        }
        std::cout << "sockInfo->listener started" << std::endl;

        // bind sockInfo->listener socket
        std::cout << "bind listener starting" << std::endl;
        if (bind(sockInfo->listener, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) < 0)
        {
            printErrMsg("host", "fail to bind listener");
            close(sockInfo->listener);
            return false;
        }
        std::cout << "listener binded" << std::endl;

        std::cout << "listening starting" << std::endl;
        if (listen(sockInfo->listener, 1) == -1)
        {
            printErrMsg("host", "fail to listen");
            close(sockInfo->listener);
            unlink(channel_name);
            return false;
        }
        std::cout << "listening started" << std::endl;

        std::cout << "accept listener starting" << std::endl;
        sockInfo->sock = accept(sockInfo->listener, NULL, NULL);
        if (sockInfo->sock == -1)
        {
            printErrMsg("host", "fail to accept listener");
            close(sockInfo->listener);
            unlink(channel_name);
            return false;
        }
        std::cout << "accept listener started" << std::endl;
    }
    else
    {
        sockInfo->listener = -1;
        sockInfo->sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockInfo->sock == -1)
        {
            printErrMsg("client", "fail to create socket");
            return false;
        }

        if (connect(sockInfo->sock, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
        {
            printErrMsg("client", "fail to connect socket");
            close(sockInfo->sock);
            return false;
        }
    }

    is_open = true;
    std::cout << "Socket connection successfully created." << std::endl;
    return true;
}


bool Conn::Close()
{
    SockInfo* sockInfo = (SockInfo*)desc_;

    if (sockInfo == nullptr)
        return false;

    if (is_open && close(sockInfo->sock) == 0)
    {
        if (sockInfo->listener != -1)
        {
            if (close(sockInfo->listener) == -1)
            {
                std::cout << "ERROR: Failed to close sockInfo->listener, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!is_host || (is_host && unlink(channel_name) == 0))
        {
            std::cout << "Connection closed." << std::endl;
            is_open = false;
            return true;
        }

        std::cout << "ERROR: Connection closing failed, error: " << strerror(errno) << std::endl;
    }

    return true;
}


bool Conn::Read(void *buf, size_t count)
{
    SockInfo* sockInfo = (SockInfo*)desc_;
    if (sockInfo == nullptr)
        return false;
    if (recv(sockInfo->sock, buf, count, 0) == -1)
    {
        std::cout << "ERROR: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write(void *buf, size_t count)
{
    SockInfo* sockInfo = (SockInfo*)desc_;
    if (sockInfo == nullptr)
        return false;
    if (send(sockInfo->sock, buf, count, MSG_NOSIGNAL) == -1)
    {
        std::cout << "ERROR: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}
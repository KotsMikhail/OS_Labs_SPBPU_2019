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

Conn::Conn() : is_open(false)
{
    channel_name = "/tmp/sock_server";
    desc = new (std::nothrow) int[2]();
}

Conn::~Conn()
{
    delete[] desc;
}

void printErrMsg(const std::string& who, const std::string& msg)
{
    std::cout << "ERROR from " << who << ": " << msg << ", error is: "<< strerror(errno) << std::endl;
}

bool Conn::Open(size_t id, bool create)
{
    if (desc == nullptr)
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
            desc[1] = accept(desc[0], NULL, NULL);
            if (desc[1] == -1)
            {
                printErrMsg("host", "fail to accept the socket");
                close(desc[0]);
                unlink(channel_name);
                return false;
            }

        }
        else
        {
            if (connect(desc[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
            {
                printErrMsg("client", "fail to connect to socket");
                close(desc[1]);
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
        desc[0] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (desc[0] == -1) {
            printErrMsg("host", "fail to create listener");
            return false;
        }
        std::cout << "desc[0] started" << std::endl;

        // bind desc[0] socket
        std::cout << "bind listener starting" << std::endl;
        if (bind(desc[0], (struct sockaddr *)&saddr, SUN_LEN(&saddr)) < 0)
        {
            printErrMsg("host", "fail to bind listener");
            close(desc[0]);
            return false;
        }
        std::cout << "listener binded" << std::endl;

        std::cout << "listening starting" << std::endl;
        if (listen(desc[0], 1) == -1)
        {
            printErrMsg("host", "fail to listen");
            close(desc[0]);
            unlink(channel_name);
            return false;
        }
        std::cout << "listening started" << std::endl;

        std::cout << "accept listener starting" << std::endl;
        desc[1] = accept(desc[0], NULL, NULL);
        if (desc[1] == -1)
        {
            printErrMsg("host", "fail to accept listener");
            close(desc[0]);
            unlink(channel_name);
            return false;
        }
        std::cout << "accept listener started" << std::endl;
    }
    else
    {
        desc[0] = -1;
        desc[1] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (desc[1] == -1)
        {
            printErrMsg("client", "fail to create socket");
            return false;
        }

        if (connect(desc[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
        {
            printErrMsg("client", "fail to connect socket");
            close(desc[1]);
            return false;
        }
    }

    is_open = true;
    std::cout << "Socket connection successfully created." << std::endl;
    return true;
}


bool Conn::Close()
{
    if (is_open && close(desc[1]) == 0)
    {
        if (desc[0] != -1)
        {
            if (close(desc[0]) == -1)
            {
                std::cout << "ERROR: Failed to close desc[0], error: " << strerror(errno) << std::endl;
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
    if (recv(desc[1], buf, count, 0) == -1)
    {
        std::cout << "ERROR: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write(void *buf, size_t count)
{
    if (send(desc[1], buf, count, MSG_NOSIGNAL) == -1)
    {
        std::cout << "ERROR: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}
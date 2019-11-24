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

const char * const g_sock_path = "/tmp/sock_server";

static const std::string g_host_name = "host";
static const std::string g_client_name = "client";

int g_listener = -1, g_sock = -1;
int g_is_host, g_created = false;

void printErrMsg(const std::string& who, const std::string& msg)
{
    std::cout << "ERROR from " << who << ": " << msg << ", error is: "<< strerror(errno) << std::endl;
}

bool Conn::Open(size_t id, bool create)
{
    std::cout << "start open socket with address: " << g_sock_path << std::endl;
    struct sockaddr_un saddr{};
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, g_sock_path);

    g_is_host = create;
    if (g_created)
    {
        if (g_is_host)
        {
            g_sock = accept(g_listener, NULL, NULL);
            if (g_sock == -1)
            {
                printErrMsg(g_host_name, "fail to accept the socket");
                close(g_listener);
                unlink(g_sock_path);
                return false;
            }

        }
        else
        {
            if (connect(g_sock, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
            {
                printErrMsg(g_client_name, "fail to connect to socker");
                close(g_sock);
                return false;
            }
        }

        return true;
    }

    if (create)
    {
        if (unlink(g_sock_path) == -1)
            std::cout << "WARNING: can't unlink " << g_sock_path << std::endl;

        // create listener socket
        std::cout << "create listener starting" << std::endl;
        g_listener = socket(AF_UNIX, SOCK_STREAM, 0);
        if (g_listener == -1) {
            printErrMsg(g_host_name, "fail to create listener");
            return false;
        }
        std::cout << "listener started" << std::endl;

        // bind listener socket
        std::cout << "bind listener starting" << std::endl;
        if (bind(g_listener, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) < 0)
        {
            printErrMsg(g_host_name, "fail to bind listener");
            close(g_listener);
            return false;
        }
        std::cout << "listener binded" << std::endl;

        std::cout << "listening starting" << std::endl;
        if (listen(g_listener, 1) == -1)
        {
            printErrMsg(g_host_name, "fail to listen");
            close(g_listener);
            unlink(g_sock_path);
            return false;
        }
        std::cout << "listening started" << std::endl;

        std::cout << "accept listener starting" << std::endl;
        g_sock = accept(g_listener, NULL, NULL);
        if (g_sock == -1)
        {
            printErrMsg(g_host_name, "fail to accept listener");
            close(g_listener);
            unlink(g_sock_path);
            return false;
        }
        std::cout << "accept listener started" << std::endl;
    }
    else
    {
        g_listener = -1;
        g_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (g_sock == -1)
        {
            printErrMsg(g_client_name, "fail to create socket");
            return false;
        }

        if (connect(g_sock, (struct sockaddr *)&saddr, SUN_LEN(&saddr)) == -1)
        {
            printErrMsg(g_client_name, "fail to connect socket");
            close(g_sock);
            return false;
        }
    }

    g_created = true;
    std::cout << "Socket connection successfully created." << std::endl;
    return true;
}


bool Conn::Close()
{
    if (g_created && close(g_sock) == 0)
    {
        if (g_listener != -1)
        {
            if (close(g_listener) == -1)
            {
                std::cout << "ERROR: Failed to close listener, error: " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!g_is_host || (g_is_host && unlink(g_sock_path) == 0))
        {
            std::cout << "Connection closed." << std::endl;
            g_created = false;
            return true;
        }

        std::cout << "ERROR: Connection closing failed, error: " << strerror(errno) << std::endl;
    }

    return true;
}


bool Conn::Read(void *buf, size_t count)
{
    if (recv(g_sock, buf, count, 0) == -1)
    {
        std::cout << "ERROR: failed to read message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}


bool Conn::Write(void *buf, size_t count)
{
    if (send(g_sock, buf, count, MSG_NOSIGNAL) == -1)
    {
        std::cout << "ERROR: failed to send message, error: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}
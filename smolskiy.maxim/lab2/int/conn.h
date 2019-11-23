#ifndef CONN_H
#define CONN_H

#include <sys/types.h>

#include <string>

class Conn
{
public:
    Conn(pid_t p, bool c);
    void Write(int num);
    int Read();
    ~Conn();

private:
    pid_t pid;
    bool create;
    int fileDesc[2];

    void ReportError(const std::string &msg);
};

#endif //CONN_H

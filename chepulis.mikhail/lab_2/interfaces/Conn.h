//
// Created by misha on 24.11.2019.
//

#ifndef LAB_2_SEVER__CONN_H
#define LAB_2_SEVER__CONN_H


#include <stdlib.h>
#include <string>

class Conn
{
public:
    Conn() = default;
    ~Conn() = default;

    bool Open(size_t id, bool create);
    bool Close();
    bool Read(void *buf, size_t count);
    bool Write(void *buf, size_t count);
private:
    std::string filename;
    bool owner;
    int *fd;

};
#endif //LAB_2_SEVER__CONN_H

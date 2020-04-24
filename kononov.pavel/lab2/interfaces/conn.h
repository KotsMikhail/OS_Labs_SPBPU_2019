#ifndef CONN_H
#define CONN_H

#include <cstdlib>
#include <string>

class Conn {
public:
    bool Open(size_t id, bool create);

    bool Read(void *buf, size_t count);

    bool Write(void *buf, size_t count);

    bool Close();

    Conn();

    ~Conn();

private:
    bool _owner;
    int _id;
    std::string _name;
    void *_pmem;
    int _desc;
};

#endif // CONN_H

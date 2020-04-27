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
    bool m_owner;
    int m_id;
    std::string m_name;
    void *m_pmem;
    int m_desc;
};

#endif // CONN_H

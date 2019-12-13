#include "conn.h"
#include "Message.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>

std::map<int, mqd_t> queues;
std::map<int, const char*> names;

void conn::Open(size_t id) {
    struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(message), 0, {0}});
    std::string name = "/lab2_queue" + std::to_string(id);
    mqd_t mqid = mq_open(name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    queues.insert(std::pair<int, mqd_t>(id, mqid));
    const char* name_const = name.c_str();
    names.insert(std::pair<int, const char*>(id, name_const));
}

void conn::Read(size_t id, void *buf, size_t count) {
    mq_receive(queues[id], (char *) buf, count, nullptr);
}

void conn::Write(size_t id, void *buf, size_t count) {
    if (count <= sizeof(message)) {
        mq_send(queues[id], (char *) buf, count, 0);
    }
}

void conn::Close(size_t id) {
    mq_unlink(names[id]);
    mq_close(queues[id]);
}

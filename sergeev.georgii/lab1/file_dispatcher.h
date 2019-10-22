//
// Created by nero on 10.10.2019.
//

#ifndef LAB1_FILE_DISPATCHER_H
#define LAB1_FILE_DISPATCHER_H

#include <sys/syslog.h>
#include <string>
enum Order{
    YOUNG_TO_OLD,
    OLD_TO_YOUNG,
};

class FileDispatcher
{
public:

    FileDispatcher(const std::string &young_dir, const std::string &old_dir, int period)
    {
        this->young_dir = young_dir;
        this->old_dir = old_dir;
        this->interval = period;
    }

    void process_directories(const std::string& src_dir, const std::string& dst_dir, Order order);

    int get_interval() {  return interval;  }

    std::string get_young_dir() { return young_dir; }

    std::string get_old_dir() { return old_dir; }

    void update(const std::string &y, const std::string &o, int i)
    {
        this->young_dir = y;
        this->old_dir = o;
        this->interval = i;
        syslog(LOG_NOTICE, "Daemon period updated to %i", interval);
    }

private:
    std::string young_dir;
    std::string old_dir;
    int interval;
    const int DEADLINE = 600;
    int move_file(const std::string& path_src, const std::string& path_dst);
};


#endif //LAB1_FILE_DISPATCHER_H

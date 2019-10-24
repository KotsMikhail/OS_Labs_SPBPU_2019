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
    static FileDispatcher *get_instance()
    {
        if (instance == nullptr){
            instance = new FileDispatcher();
        }
        return instance;
    }

    static void destroy()
    {
        delete instance;
    }

    void process_directories(const std::string& src_dir, const std::string& dst_dir, Order order);

    void process();

    std::string get_young_dir() { return young_dir; }

    std::string get_old_dir() { return old_dir; }

    std::string get_config_file_path() { return config_file_path; }

    void set_config_file_path(std::string path) { config_file_path = path; }

    void update(const std::string &y, const std::string &o)
    {
        this->young_dir = y;
        this->old_dir = o;
        //syslog(LOG_NOTICE, "Daemon period updated to %i", interval);
    }

private:
    static FileDispatcher *instance;
    std::string young_dir;
    std::string old_dir;
    std::string config_file_path;
    const int DEADLINE = 600;
    int move_file(const std::string& path_src, const std::string& path_dst);
    FileDispatcher()
    {
        this->young_dir = "";
        this->old_dir = "";
    }
    FileDispatcher(FileDispatcher const&) = delete;
    FileDispatcher& operator=(FileDispatcher const&) = delete;
};


#endif //LAB1_FILE_DISPATCHER_H

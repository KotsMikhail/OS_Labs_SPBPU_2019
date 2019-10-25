//                        return;

// Created by nero on 24.10.2019.
//

#include <syslog.h>
#include <dirent.h>
#include <cstring>
#include "file_dispatcher.h"
#include <sys/sendfile.h>
#include <fcntl.h>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>

FileDispatcher * FileDispatcher::instance = nullptr;

void FileDispatcher::process_directories(const std::string &src_dir, const std::string &dst_dir, Order order)
{
    DIR *dir;
    struct dirent *entry_ptr = nullptr;
    time_t creation_time, cur_time;
    time(&cur_time);
    if ((dir = opendir(src_dir.c_str())) != nullptr)
    {
        entry_ptr = readdir(dir);
        while (entry_ptr != nullptr)
        {
            struct stat entry_stat;
            if ((strncmp(entry_ptr->d_name, ".", PATH_MAX) == 0) ||
                (strncmp(entry_ptr->d_name, "..", PATH_MAX) == 0))
            {
                /* don't get in scope '.' and '..' entries. */
                entry_ptr = readdir(dir);
                continue;
            }
            std::string src_path = src_dir + std::string("/") + std::string(entry_ptr->d_name);
            std::string dst_path = dst_dir + std::string("/") + std::string(entry_ptr->d_name);
            if (stat(src_path.c_str(), &entry_stat) == 0)
            {
                if (S_ISREG(entry_stat.st_mode))
                {
                    creation_time = entry_stat.st_atime;
                    switch (order)
                    {
                        case YOUNG_TO_OLD:
                            if (cur_time - creation_time <= this->DEADLINE)
                            {
                                entry_ptr = readdir(dir);
                                continue;
                            }
                            break;
                        case OLD_TO_YOUNG:
                            if (cur_time - creation_time >= this->DEADLINE)
                            {
                                entry_ptr = readdir(dir);
                                continue;
                            }
                            break;
                    }
                    if (move_file(src_path, dst_path) != EXIT_SUCCESS || remove(src_path.c_str()) != 0)
                    {
                        syslog(LOG_ERR, "Could not move file");
                        destroy();
                        exit(EXIT_FAILURE);
                    }
                }
            }
            entry_ptr = readdir(dir);
        }
        closedir(dir);

    }
    else
    {
        syslog(LOG_ERR, "Could not process files in directory");
        destroy();
        exit(EXIT_FAILURE);
    }
}

int FileDispatcher::move_file(const std::string &src, const std::string &dst)
{
    struct stat source_info;
    stat(src.c_str(), &source_info);
    int src_size = source_info.st_size;
    int source_file = open(src.c_str(), O_RDONLY, 0);
    int destination_file = open(dst.c_str(), O_WRONLY | O_CREAT, 0644);
    int sent_count = sendfile(destination_file, source_file, nullptr, src_size);
    close(source_file);
    close(destination_file);
    if (sent_count != src_size)
    {
        syslog(LOG_ERR, "Not all bytes of source file were sent");
        return EXIT_FAILURE;
    }
    //syslog(LOG_NOTICE, "File %s successfully moved to %s", src.c_str(), dst.c_str());
    return EXIT_SUCCESS;
}

void FileDispatcher::process() {
    std::string young_dir = get_young_dir();
    std::string old_dir = get_old_dir();
    if (young_dir.compare(old_dir) == 0)
    {
        syslog(LOG_ERR, "Directories are equal");
        destroy();
        exit(EXIT_FAILURE);
    }
    process_directories(old_dir, young_dir, OLD_TO_YOUNG);
    process_directories(young_dir, old_dir, YOUNG_TO_OLD);
}


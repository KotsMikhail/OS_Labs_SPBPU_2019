#ifndef FILE_DELETER_H
#define FILE_DELETER_H

#include <iostream>
#include <ctime>
#include <syslog.h>
#include <dirent.h>
#include <sys/stat.h>

class FileDeleter {
public:
    void setPar(std::string path) {
        this->path = path;
    }

    void exec();

private:
    std::string path;
    const double treshold_seconds = 60.0;
};

#endif

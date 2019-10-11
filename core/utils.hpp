//
// Created by dmitrii on 04.10.2019.
//

#ifndef INOTIFY_UTILS_HPP
#define INOTIFY_UTILS_HPP

#include <unistd.h>
#include <climits>
#include <iostream>

namespace utils {
    static std::string getCurrentPath(){
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            return std::string(cwd);
        }
    }
}

#endif //INOTIFY_UTILS_HPP

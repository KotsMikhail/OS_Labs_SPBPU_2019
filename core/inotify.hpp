//
// Created by dmitrii on 04.10.2019.
//

#ifndef INOTIFY_INOTIFY_HPP
#define INOTIFY_INOTIFY_HPP

#include <iostream>
#include <map>
#include <sys/inotify.h>
#include "ConfigHolder.h"
#include <utility>

class inotify {
public:
    static std::map<int, std::string> addWatchers(int fd) {
        std::map<int, std::string> res = std::map<int, std::string>();

        ConfigHolder *ch = ConfigHolder::getInstance();
        for (auto &watch_dir : ch->get("watch_directories")) {
            int wd = inotify_add_watch(fd, watch_dir.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
            if (wd == -1) {
                syslog(LOG_ERR, "can't add watcher for inotify");
                throw std::exception();
            }
            res.insert(std::pair<int, std::string>(wd, watch_dir));
        }
        return res;
    }
};


#endif //INOTIFY_INOTIFY_HPP

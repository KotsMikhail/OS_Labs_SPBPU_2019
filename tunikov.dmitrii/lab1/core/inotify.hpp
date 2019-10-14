//
// Created by dmitrii on 04.10.2019.
//

#ifndef INOTIFY_INOTIFY_HPP
#define INOTIFY_INOTIFY_HPP

#include <iostream>
#include <map>
#include <sys/inotify.h>
#include "ConfigHolder.h"
#include "Exception.hpp"
#include <unistd.h>
#include <utility>

using namespace std;
#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))

class inotifier {
private:
    char *buf;
    int fd;
    std::map<int, std::string> watchers;
    bool is_running;

    void addWatchers() {
        ConfigHolder *ch = ConfigHolder::getInstance();
        for (auto &watch_dir : ch->get("watch_directories")) {
            int wd = inotify_add_watch(fd, watch_dir.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
            if (wd == -1) {
                throw CommonException("can't add watcher for inotify");
            }
            watchers.insert(std::pair<int, std::string>(wd, watch_dir));
        }
    }

    void removeWatchers()
    {
        for (auto& watch_dir : watchers)
        {
            int wd = inotify_rm_watch(fd, watch_dir.first);
            if (wd == -1)
                throw CommonException("can't delete watcher for inotify");
            watchers.erase(watch_dir.first);
        }
    }

    inotifier(int fd_, char *buff_) : fd(fd_), buf(buff_), watchers(std::map<int, std::string>())
    {
    }

public:
    ~inotifier(){
        delete[] buf;
    }

    static inotifier* createNotifier()
    {
        int fd = inotify_init();
        char *buf = new char[BUF_LEN];

        if ( fd < 0 ) {
            throw CommonException("can't init inotify");
        }

        auto* obj = new inotifier(fd, buf);
        try {
            obj->addWatchers();
        }
        catch (CommonException& e)
        {
            delete[] buf;
            delete obj;
            throw CommonException(e);
        }
        obj->is_running = true;
        return obj;
    }

    void printWatchDirs()
    {
        std::string wds = "";
        for (auto& wd : watchers)
            wds += wd.second + "; ";
        syslog(LOG_LOCAL0, "new watch dirs: %s", wds.c_str());
    }

    void reloadNotifier()
    {
        is_running = false;
        removeWatchers();
        addWatchers();
        is_running = true;
    }

    int runNotifier() {
        syslog(LOG_LOCAL0, "start watching direcrories");
        while (true) {
            if (!is_running)
                continue;

            int offset = 0;
            int len = read(fd, buf, BUF_LEN);
            syslog(LOG_LOCAL0, "change something");
            if (len == -1) {
                throw CommonException("can't read buff from fd");
            }

            auto *event = (inotify_event *) &buf[offset];

            while (offset < len) {
                if (event->len) {
                    if (event->mask & IN_CREATE) {
                        if (event->mask & IN_ISDIR)
                            syslog(LOG_LOCAL0, "The directory %s was Created into watch_dir %s\n", event->name,
                                   watchers.find(event->wd)->second.c_str());
                        else {
                            syslog(LOG_LOCAL0, "The file %s was Created into watch_dir %s\n", event->name,
                                   watchers.find(event->wd)->second.c_str());
                        }
                    }

                    if (event->mask & IN_MODIFY) {
                        if (event->mask & IN_ISDIR)
                            syslog(LOG_LOCAL0, "The directory %s was modified into watch_dir %s\n", event->name,
                                   watchers.find(event->wd)->second.c_str());
                        else
                            syslog(LOG_LOCAL0, "The file %s was modified into watch_dir %s\n", event->name,
                                   watchers.find(event->wd)->second.c_str());
                    }

                    if (event->mask & IN_DELETE) {
                        if (event->mask & IN_ISDIR)
                            syslog(LOG_LOCAL0, "The directory %s was deleted into watch_dir %s\n", event->name,
                                   watchers.find(event->wd)->second.c_str());
                        else
                            syslog(LOG_LOCAL0, "The file %s was deleted into watch_dir %s\n", event->name,
                                   watchers.find(event->wd)->second.c_str());
                    }

                    offset += EVENT_SIZE * event->len;
                }
            }
        }
    }
};


#endif //INOTIFY_INOTIFY_HPP

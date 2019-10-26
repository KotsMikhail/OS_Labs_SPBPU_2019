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
static const int MAX_EVENTS = 1024;
static const int LEN_NAME =  16;
static const int EVENT_SIZE =  (sizeof (struct inotify_event));
static const int BUF_LEN = (MAX_EVENTS * (EVENT_SIZE + LEN_NAME));

class inotifier
{
private:
    char *buf;
    int fd;
    std::map<int, std::string> watchers;

    void addWatchers()
    {
        ConfigHolder *ch = ConfigHolder::getInstance();
        for (auto &watch_dir : ch->get("watch_directories"))
        {
            int wd = inotify_add_watch(fd, watch_dir.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
            if (wd == -1)
            {
                throw CommonException("can't add watcher for inotify");
            }
            watchers.insert(std::pair<int, std::string>(wd, watch_dir));
        }
    }

    void removeWatchers()
    {
        auto itr = watchers.begin();
        while (itr != watchers.end())
        {
            int wd = inotify_rm_watch(fd, itr->first);
            if (wd == -1)
                throw CommonException("can't delete watcher for inotify");
            watchers.erase(itr++);
        }
    }

    inotifier(int fd_, char *buff_) : fd(fd_), buf(buff_), watchers(std::map<int, std::string>())
    {
    }

    void logEvent(const char* event_name, const char* place_name, const char* file_name, const char* watch_dir_name, uint32_t* cookie = nullptr)
    {
        if (cookie != nullptr)
        {
            syslog(LOG_LOCAL0, "action: %s, with %s %s into %s, cookie: %u", event_name, place_name, file_name, watch_dir_name, *cookie);
        }
        else
        {
            syslog(LOG_LOCAL0, "action: %s, with %s %s into %s", event_name, place_name, file_name, watch_dir_name);
        }
    }

public:
    ~inotifier()
    {
        delete[] buf;
    }

    static inotifier* createNotifier()
    {
        int fd = inotify_init();
        char *buf = new char[BUF_LEN];

        if ( fd < 0 )
        {
            throw CommonException("can't init inotify");
        }

        auto* obj = new inotifier(fd, buf);
        try {
            obj->addWatchers();
        }
        catch (CommonException& e)
        {
            delete obj;
            throw CommonException(e);
        }
        return obj;
    }

    void reloadNotifier()
    {
        removeWatchers();
        addWatchers();
    }

    int runNotifier()
    {
        while (true)
        {
            int len = read(fd, buf, BUF_LEN);
            if (len <= 0)
            {
                throw CommonException("can't read len to buffer from FD");
            }
            int offset = 0;
            while (offset < len)
            {
                auto *event = (inotify_event *) &buf[offset];
                char place_name[10];
                if (event->mask & IN_ISDIR)
                    sprintf(place_name, "directory");
                else
                    sprintf(place_name, "file");

                uint32_t cookie = event->cookie;
                const char *dir_name = nullptr;
                auto it = watchers.find(event->wd);
                if (it != watchers.end())
                    dir_name = it->second.c_str();

                switch (event->mask & (IN_ALL_EVENTS | IN_IGNORED))
                {
                    case IN_OPEN:
                        logEvent("OPEN", place_name, event->name, dir_name);
                        break;
                    case IN_CREATE:
                        logEvent("CREATE", place_name, event->name, dir_name);
                        break;
                    case IN_MODIFY:
                        logEvent("MODIFY", place_name, event->name, dir_name);
                        break;
                    case IN_DELETE:
                        logEvent("DELETE", place_name, event->name, dir_name);
                        break;
                    case IN_ACCESS:
                        logEvent("ACCESS", place_name, event->name, dir_name);
                        break;
                    case IN_ATTRIB:
                        logEvent("ATTRIB", place_name, event->name, dir_name);
                        break;
                    case IN_CLOSE_WRITE:
                        logEvent("CLOSE_WRITE", place_name, event->name, dir_name);
                        break;
                    case IN_CLOSE_NOWRITE:
                        logEvent("CLOSE_NOWRITE", place_name, event->name, dir_name);
                        break;
                    case IN_MOVED_FROM:
                        logEvent("MOVED_FROM", place_name, event->name, dir_name, &cookie);
                        break;
                    case IN_MOVED_TO:
                        logEvent("MOVED_TO", place_name, event->name, dir_name, &cookie);
                        break;
                    case IN_DELETE_SELF:
                        logEvent("DELETE_SELF", place_name, event->name, dir_name);
                        break;
                    case IN_MOVE_SELF:
                        logEvent("MOVE_SELF", place_name, event->name, dir_name);
                        break;
                    case IN_IGNORED:
                        syslog(LOG_NOTICE, "IGNORED");
                        break;
                    default:
                        syslog(LOG_NOTICE, "UNKNOWN EVENT for file %s in %s", event->name, dir_name);
                        break;
                }
                offset += EVENT_SIZE + event->len;
            }
        }
    }
};


#endif //INOTIFY_INOTIFY_HPP

#ifndef DISK_MONITOR_INOTIFY_H
#define DISK_MONITOR_INOTIFY_H

#include <sys/inotify.h>

#include "types.h"

class inotify_t
{
public:
    inotify_t();
    ~inotify_t();

    void init();
    void do_inotify();
    void add_watchers(const set_string_t & directories_to_add);
    void remove_watchers(const set_string_t & directories_to_rm);

private:
    int _fd;
    map_int_string_t _watch_directories;

    void handle_event(inotify_event *event);
};

#endif // DISK_MONITOR_INOTIFY_H
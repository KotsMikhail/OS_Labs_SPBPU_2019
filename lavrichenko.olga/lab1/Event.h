#ifndef LAB1_EVENT_H
#define LAB1_EVENT_H


#include <cstdlib>
#include <sys/stat.h>
#include <ctime>
#include <fcntl.h>
#include <list>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <syslog.h>

typedef enum {w = 30, d = 86400, h = 3600, NONE = 0} EventType; // 604800

class Event {
public:
    Event(tm time, std::string text, EventType t) {
        _time = time;
        _text = text;
        _repeatTime = t;
        _done = false;
        _last_remind = 0;
    }

    bool checkTime(time_t &currTime);

    std::string getText() { return _text; }

private:
    tm _time;
    time_t _last_remind;
    bool _done;
    std::string _text;
    EventType _repeatTime;
};


#endif //LAB1_EVENT_H

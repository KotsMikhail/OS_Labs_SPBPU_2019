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
        this->time = time;
        this->text = text;
        repeatTime = t;
        done = false;
        last_remind = 0;
    }

    bool checkTime(time_t &currTime);

    std::string getText() { return text; }

private:
    tm time;
    time_t last_remind;
    bool done;
    std::string text;
    EventType repeatTime;
};


#endif //LAB1_EVENT_H

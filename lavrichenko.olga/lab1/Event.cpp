#include "Event.h"
static const int SLEEP_TIME = 1;


bool Event::checkTime(time_t &currTime) {
    double diff = std::difftime(std::mktime(&time), currTime);
    if (!done) {
        if (std::abs(diff) < 4 * SLEEP_TIME) {
            done = true;
            return true;
        }
        return false;
    }

    if (last_remind == 0) {
        last_remind = std::mktime(&time);
    }

    if (repeatTime != NONE) {
        if (last_remind + repeatTime < currTime) {
            last_remind += repeatTime * ((currTime - last_remind) / repeatTime);
            return true;
        }
    }

    return false;
}
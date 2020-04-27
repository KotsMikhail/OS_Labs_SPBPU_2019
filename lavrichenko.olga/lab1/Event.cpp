#include "Event.h"
static const int SLEEP_TIME = 1;


bool Event::checkTime(time_t &currTime) {
    double diff = std::difftime(std::mktime(&_time), currTime);
    if (!_done) {
        if (std::abs(diff) < 4 * SLEEP_TIME) {
            _done = true;
            return true;
        }
        return false;
    }

    if (_last_remind == 0) {
        _last_remind = std::mktime(&_time);
    }

    if (_repeatTime != NONE) {
        if (_last_remind + _repeatTime < currTime) {
            _last_remind += _repeatTime * ((currTime - _last_remind) / _repeatTime);
            return true;
        }
    }

    return false;
}
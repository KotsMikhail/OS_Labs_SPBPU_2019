#include "Reminder.h"

void Reminder::exec() {
    std::time_t t = std::time(0);   // get time now
    for (auto it = events->begin(); it != events->end(); it++) {
        if ((*it).checkTime(t))
            printText((*it).getText());
    }
}


void Reminder::printText(std::string text) {
    std::string query = _form + text;
    system(query.c_str());
}


Reminder &Reminder::getInstance() {
    static Reminder instance;
    return instance;
}


void Reminder::read() {
    delete events;
    events = config_reader.read();
}


void Reminder::setPath(std::string &str) {
    config_reader.setPath(str);
    events = config_reader.read();
}
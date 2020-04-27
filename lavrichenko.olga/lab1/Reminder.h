#ifndef LAB1_REMINDER_H
#define LAB1_REMINDER_H


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

#include "ConfigReader.h"
#include "Event.h"


class Reminder {
public:
    void exec();

    void printText(std::string text);

    void setPath(std::string &str);

    static Reminder &getInstance();

    void read();

    Reminder() : _form("gnome-terminal -- ") {
        std::string _print_text_path = realpath("print_text", nullptr);
        _form += _print_text_path + " ";
        events = nullptr;
    }

    ~Reminder() {
        delete events;
    }

private:
    std::string _form;
    std::list<Event> *events;
    ConfigReader config_reader;
};


#endif //LAB1_REMINDER_H

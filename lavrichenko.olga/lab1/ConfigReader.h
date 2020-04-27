#ifndef LAB1_CONFIGREADER_H
#define LAB1_CONFIGREADER_H

#include <fstream>
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
#include "Event.h"


class ConfigReader {
public:
    void setPath(std::string path) { config_path = path; }

    std::list<Event> *read();

private:
    std::string config_path;

    static bool canParseLine(std::string &line);

    static bool isDateTime(std::string &str);

    static Event parseEvent(std::string &line);
};


#endif //LAB1_CONFIGREADER_H

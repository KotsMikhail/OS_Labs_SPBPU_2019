#include "ConfigReader.h"

bool ConfigReader::isDateTime(std::string& str) {
    std::stringstream ss(str);
    tm time;
    ss >> std::get_time(&time, "%d.%m.%Y-%H:%M:%S");
    return !ss.fail();
}


bool ConfigReader::canParseLine(std::string & line) {
    std::stringstream ss(line);
    std::string token;
    ss >> token;
    if (token != "add_event")
        return false;
    ss >> token;
    return isDateTime(token);
}


Event ConfigReader::parseEvent(std::string &line) {
    tm time;
    EventType t = NONE;
    std::string text;

    std::stringstream ss(line);
    std::string token;
    ss >> token;
    ss >> std::get_time(&time, "%d.%m.%Y-%H:%M:%S");
    ss >> token;
    if (token == "-w")
        t = w;
    else if (token == "-h")
        t = h;
    else if (token == "-d")
        t = d;
    else
        text += token;

    while (!ss.eof()) {
        ss >> token;
        if (!text.empty())
            text += " ";
        text += token;
    }
    return Event(time, text, t);
}

std::list<Event> *ConfigReader::read() {
    auto *events = new std::list<Event>;
    std::ifstream config = std::ifstream(config_path);
    if (!config.is_open() || config.eof()) {
        syslog(LOG_ERR, "Bad path or empty config");
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(config, line)) {
        if (canParseLine(line))
            events->push_back(parseEvent(line));
    }

    config.close();
    return events;
}


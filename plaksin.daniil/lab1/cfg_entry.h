#ifndef CFG_ENTRY_H
#define CFG_ENTRY_H

#include <string>
#include <ctime>

class cfg_entry
{
private:
    std::string entry_text;
    std::string entry_flag;
    tm entry_time;

public:
    cfg_entry(std::string text, std::string flag, tm time);
    std::string get_entry_text();
    std::string get_entry_flag();
    tm get_entry_time();
};

#endif //CFG_ENTRY_H
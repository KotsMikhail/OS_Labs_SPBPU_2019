#include "cfg_entry.h"

cfg_entry::cfg_entry(std::string text, std::string flag, tm time): entry_text(text), entry_flag(flag), entry_time(time) {}

std::string cfg_entry::get_entry_text()
{
    return entry_text;
}

std::string cfg_entry::get_entry_flag()
{
    return entry_flag;
}

tm cfg_entry::get_entry_time()
{
    return entry_time;
}
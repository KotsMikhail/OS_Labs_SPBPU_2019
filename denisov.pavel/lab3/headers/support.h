#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include <string>

bool        isDir        (const std::string &path);
void        initHomeDir  (void);
std::string getRealPath  (std::string &path);
bool        fromStrToInt (const std::string &str, int &res);

#endif //__SUPPORT_H__

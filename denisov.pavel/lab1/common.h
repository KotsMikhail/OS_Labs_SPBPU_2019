#ifndef __COMMON_H__
#define __COMMON_H__

#include <syslog.h>

#define WARNING(...) do{ syslog(LOG_WARNING, ##__VA_ARGS__); } while(0)
#define ERROR(...)   do{ syslog(    LOG_ERR, ##__VA_ARGS__); } while(0)
#define INFO(...)    do{ syslog(   LOG_INFO, ##__VA_ARGS__); } while(0)

#define LOG_ERROR_AND_EXIT(...) \
    do {                        \
        ERROR(__VA_ARGS__);     \
        exit(SIGTERM);          \
    } while (0)

#endif //__COMMON_H__

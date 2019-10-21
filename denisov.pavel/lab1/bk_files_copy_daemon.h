#ifndef __BK_FILES_COPY_DAEMON_H__
#define __BK_FILES_COPY_DAEMON_H__

#include <string>

void Daemonise      ();
void DaemonWorkLoop ();
bool LoadConfig     (const std::string &configFilePath);

#endif //__BK_FILES_COPY_DAEMON_H__

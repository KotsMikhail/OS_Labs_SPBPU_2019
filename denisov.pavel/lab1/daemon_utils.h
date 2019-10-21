#ifndef __DAEMON_UTILS_H__
#define __DAEMON_UTILS_H__

#include <cstdio>
#include <string>
#include <sys/types.h>

void  Fork                      ();
pid_t CreateSession             ();
void  RedirectStdIO             ();
bool  IsProcessRunning          (pid_t pid);
bool  IsDirectoryExist          (std::string &dirPath);
void  CreatePidFileAndWritePid  (const std::string &pidFilePath);
void  RemovePidFile             (const std::string &pidFilePath);
void  StopRunningByPID          (pid_t pid);
void  CheckPidFile              (const std::string &pidFilePath);
void  SetRootAsWorkingDirectory ();

#endif //__DAEMON_UTILS_H__

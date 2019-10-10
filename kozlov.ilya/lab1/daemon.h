//
// Created by ilya on 10/10/19.
//

#ifndef LAB1__DAEMON_H_
#define LAB1__DAEMON_H_

#include <string>

class Daemon
{
private:
  static std::string pid_file_;
  static std::string config_file_;
  static int time_interval_;
  static std::string name_;

public:
  static bool Init(char* config_file);
  static void Start();

private:
  static void Terminate();
  static bool LoadConfig();
  static bool CheckPidFile();
  static bool SetPidFile();
  static void SetSignals();
  static void StartWork();
  static void DoWork();
  static void SignalHandler(int sig_num);
};

#endif //LAB1__DAEMON_H_

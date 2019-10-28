//
// Created by peter on 10/25/19.
//

#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H

#include <string>
#include <vector>

class Daemon
{
private:
  using uint = unsigned int;
  using string = std::string;
  using svector = std::vector<string>;

  // constants
  static const string LOG_TAG;
  static const string INTERVAL_PARAM_NAME;
  static const string DIR1_PARAM_NAME;
  static const string DIR2_PARAM_NAME;

  // work related parameters
  uint timeInterval = 45;
  string dir1Path = "dir1";
  string dir2Path = "dir2";
  string totalLogPath;

  // daemon related parameters
  string workDir;
  string configFileName;
  string pidFileName = "/var/run/lab1.pid";

  string getAbsolutePath( const string &path );
  static void clear();
  bool handlePidFile();
  bool setPidFile();

  static void signalHandle( int sigNum );
  static void terminate();

  void parseConfig();
  void startWorkLoop();
  void doWork();

  static svector findLogFiles( const string &curDir );
  static void saveLog( const string &logFilePath, std::ofstream &totalLog );

  Daemon() = default;

public:
  Daemon( const Daemon &configParser ) = delete;
  Daemon &operator=( const Daemon &configParser ) = delete;

  static Daemon &getInstance();
  bool start( const string &configFilename );
};

#endif //LAB1_DAEMON_H

// END OF 'daemon.h' FILE

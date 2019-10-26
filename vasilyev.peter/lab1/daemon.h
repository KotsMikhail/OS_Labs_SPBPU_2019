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

  // work related parameters
  static uint timeInterval;
  static string dir1Path;
  static string dir2Path;
  static string totalLogPath;

  // daemon related parameters
  static const string LOG_TAG;
  static string workDir;
  static string configFileName;
  static string pidFileName;

public:
  static bool start( const string &configFilename );

private:
  static string getAbsolutePath( const string &path );
  static void clear();
  static bool handlePidFile();
  static bool setPidFile();

  static void signalHandle( int sigNum );
  static void terminate();

  static bool parseConfig();
  static void startWorkLoop();
  static void doWork();

  static svector findLogFiles( const string &curDir );
  static void saveLog( const string &logFilePath, std::ofstream &totalLog );
};

#endif //LAB1_DAEMON_H

// END OF 'daemon.h' FILE

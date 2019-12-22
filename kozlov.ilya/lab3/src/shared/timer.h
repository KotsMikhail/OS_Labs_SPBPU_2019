#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <ctime>

class Timer
{
private:
  timespec beg, end;
public:
  const double TIMEOUT = 1.0;

  Timer()
  {
    clock_gettime(CLOCK_REALTIME, &beg);
  }

  double elapsed()
  {
    clock_gettime(CLOCK_REALTIME, &end);
    return end.tv_sec - beg.tv_sec +
           (end.tv_nsec - beg.tv_nsec) / 1000000000.;
  }

  void reset()
  {
    clock_gettime(CLOCK_REALTIME, &beg);
  }
};

#endif // TIMER_H

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
  std::chrono::steady_clock::time_point start_time;
public:
  Timer();
  void start();
  double elapsed() const;
  
  static Timer create();
};

#endif  // TIMER_H

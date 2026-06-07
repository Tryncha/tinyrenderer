#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

class Timer {
 public:
  void reset() { m_beg = clock::now(); }

  double elapsed() const {
    return std::chrono::duration_cast<second>(clock::now() - m_beg).count();
  }

  void print_time_elapsed() const {
    std::cout << "Time elapsed: " << this->elapsed() << " seconds\n";
  }

 private:
  using clock = std::chrono::steady_clock;
  using second = std::chrono::duration<double, std::ratio<1>>;

  std::chrono::time_point<clock> m_beg{clock::now()};
};

#endif

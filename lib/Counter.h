#pragma once

#include <atomic>

namespace prometheus {
class Counter
{
  public:
    Counter();
    void increment(double);
  private:
    std::atomic<double> value_;
};
}

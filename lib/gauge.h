#pragma once

#include <atomic>

namespace prometheus {

class Gauge {
 public:
  Gauge();
  Gauge(double);
  void inc();
  void inc(double);
  void dec();
  void dec(double);
  void set(double);
  void set_to_current_time();
  double value() const;

 private:
  void change(double);
  std::atomic<double> value_;
};
}

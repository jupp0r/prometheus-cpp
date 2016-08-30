#pragma once

#include <atomic>

#include "gauge.h"

namespace prometheus {
class Counter {
 public:
  void inc();
  void inc(double);
  double value() const;

 private:
  Gauge gauge_;
};
}

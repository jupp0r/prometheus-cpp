#pragma once

#include <atomic>

#include "gauge.h"
#include "metric.h"

namespace prometheus {
class Counter : Metric {
 public:
  void inc();
  void inc(double);
  double value() const;

  io::prometheus::client::Metric collect();

 private:
  Gauge gauge_;
};
}

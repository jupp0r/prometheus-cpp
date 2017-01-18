#pragma once

#include <atomic>

#include "cpp/metrics.pb.h"

#include "gauge.h"
#include "metric.h"

namespace prometheus {
class Counter : Metric {
 public:
  static const io::prometheus::client::MetricType metric_type =
      io::prometheus::client::COUNTER;

  void Increment();
  void Increment(double);
  double Value() const;

  io::prometheus::client::Metric Collect();

 private:
  Gauge gauge_;
};
}

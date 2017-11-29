#pragma once

#include <atomic>

#include "metrics.pb.h"

#include "prometheus/gauge.h"
#include "prometheus/metric.h"

namespace prometheus {
class Counter : Metric {
 public:
  static const io::prometheus::client::MetricType metric_type =
      io::prometheus::client::COUNTER;

  void Increment();
  void Increment(double);
  double Value() const;
  void Set(double value);

  io::prometheus::client::Metric Collect();

 private:
  Gauge gauge_;
};
}

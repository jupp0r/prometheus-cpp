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

  void inc();
  void inc(double);
  double value() const;

  io::prometheus::client::Metric collect();

 private:
  Gauge gauge_;
};
}

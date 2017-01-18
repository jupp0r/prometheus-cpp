#pragma once

#include <atomic>

#include "collectable.h"
#include "cpp/metrics.pb.h"
#include "metric.h"

namespace prometheus {

class Gauge : public Metric {
 public:
  static const io::prometheus::client::MetricType metric_type =
      io::prometheus::client::GAUGE;

  Gauge();
  Gauge(double);
  void Increment();
  void Increment(double);
  void Decrement();
  void Decrement(double);
  void Set(double);
  void SetToCurrentTime();
  double Value() const;

  io::prometheus::client::Metric Collect();

 private:
  void Change(double);
  std::atomic<double> value_;
};
}

#pragma once

#include <atomic>

#include "prometheus/client_metric.h"
#include "prometheus/collectable.h"
#include "prometheus/metric.h"

namespace prometheus {

class Gauge {
 public:
  static const MetricType metric_type = MetricType::Gauge;

  Gauge();
  Gauge(double);
  void Increment();
  void Increment(double);
  void Decrement();
  void Decrement(double);
  void Set(double);
  void SetToCurrentTime();
  double Value() const;

  ClientMetric Collect();

 private:
  void Change(double);
  std::atomic<double> value_;
};
}

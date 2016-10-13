#pragma once

#include <atomic>

#include "cpp/metrics.pb.h"
#include "metric.h"

namespace prometheus {

class Gauge : public Metric {
 public:
  static const io::prometheus::client::MetricType metric_type =
      io::prometheus::client::GAUGE;

  Gauge();
  Gauge(double);
  void inc();
  void inc(double);
  void dec();
  void dec(double);
  void set(double);
  void set_to_current_time();
  double value() const;

  io::prometheus::client::Metric collect();

 private:
  void change(double);
  std::atomic<double> value_;
};
}

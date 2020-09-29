#include "prometheus/gauge.h"

#include <atomic>
#include <ctime>

namespace prometheus {

Gauge::Gauge(const double value, const bool alert_if_no_family) : MetricBase(alert_if_no_family), value_(value) {}

void Gauge::Reset(const double value) {
  value_ = value;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

void Gauge::Increment(const double value) {
  if (value < 0.0) return;
  value_ = value_ + value;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

void Gauge::Decrement(const double value) {
  if (value < 0.0) return;
  value_ = value_ - value;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

void Gauge::Set(const double value) {
  value_ = value;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

void Gauge::SetToCurrentTime() {
  const auto time = std::time(nullptr);
  Set(static_cast<double>(time));
}

double Gauge::Value() const { return value_; }

ClientMetric Gauge::Collect() const {
  ClientMetric metric;
  metric.gauge.value = value_;
  return metric;
}

}  // namespace prometheus

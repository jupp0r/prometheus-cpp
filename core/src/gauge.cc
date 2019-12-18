#include "prometheus/gauge.h"

#include <atomic>
#include <ctime>

namespace prometheus {

Gauge::Gauge(const double& value) : value_(value) {}

void Gauge::Increment(const double& value, const bool& alert) {
  if (value < 0.0) return;
  value_ = value_ + value;
  last_update_ = std::time(nullptr);
  if (alert) AlertIfNoFamily();
}

void Gauge::Decrement(const double& value, const bool& alert) {
  if (value < 0.0) return;
  value_ = value_ - value;
  last_update_ = std::time(nullptr);
  if (alert) AlertIfNoFamily();
}

void Gauge::Set(const double& value, const bool& alert) {
  value_ = value;
  last_update_ = std::time(nullptr);
  if (alert) AlertIfNoFamily();
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

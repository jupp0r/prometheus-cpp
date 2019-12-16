#include "prometheus/gauge.h"

#include <ctime>

namespace prometheus {

Gauge::Gauge(const double& value) : value_(value) {}

void Gauge::Increment() { Increment(1.0); }

void Gauge::Increment(const double value) {
  if (value < 0.0) {
    return;
  }
  Change(value);
}

void Gauge::Decrement() { Decrement(1.0); }

void Gauge::Decrement(const double value) {
  if (value < 0.0) {
    return;
  }
  Change(-1.0 * value);
}

void Gauge::Set(const double value) {
  value_.store(value);
  last_update_.store(std::time(nullptr));
}

void Gauge::Change(const double value) {
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value))
    ;
  last_update_.store(std::time(nullptr));
}

void Gauge::SetToCurrentTime() {
  const auto time = std::time(nullptr);
  Set(static_cast<double>(time));
  last_update_.store(std::time(nullptr));
}

double Gauge::Value() const { return value_; }

ClientMetric Gauge::Collect() const {
  ClientMetric metric;
  metric.gauge.value = Value();
  return metric;
}

void Gauge::UpdateRetentionTime(const double& retention_time, const bool& bump) {
  if (bump) last_update_.store(std::time(nullptr));
  retention_time_ = retention_time; 
};

bool Gauge::Expired() const {
  return std::difftime(std::time(nullptr), last_update_) > retention_time_;
}

}  // namespace prometheus

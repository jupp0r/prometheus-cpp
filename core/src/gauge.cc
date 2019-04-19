#include "prometheus/gauge.h"

#include <ctime>

namespace prometheus {

Gauge::Gauge(const double value) : value_{value}, time_{std::time(nullptr)} {}

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
  time_.store(std::time(nullptr));
}

void Gauge::Change(const double value) {
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value))
    ;
  time_.store(std::time(nullptr));
}

void Gauge::SetToCurrentTime() {
  const auto time = std::time(nullptr);
  Set(static_cast<double>(time));
}

double Gauge::Value() const { return value_; }

ClientMetric Gauge::Collect() const {
  ClientMetric metric;
  metric.gauge.value = Value();
  return metric;
}

bool Gauge::Expired(double seconds) const {
  const auto time = std::time(nullptr);
  return std::difftime(time, time_);
}

detail::GaugeBuilder BuildGauge() { return {}; }

}  // namespace prometheus

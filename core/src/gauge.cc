#include "prometheus/gauge.h"

#include <ctime>

namespace prometheus {

Gauge::Gauge(const detail::value_type value) : value_{value} {}

void Gauge::Increment() { Increment(1.0); }

void Gauge::Increment(const detail::value_type value) { Change(value); }

void Gauge::Decrement() { Decrement(1.0); }

void Gauge::Decrement(const detail::value_type value) { Change(-1.0 * value); }

void Gauge::Set(const detail::value_type value) { value_.store(value); }

void Gauge::Change(const detail::value_type value) {
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value))
    ;
}

void Gauge::SetToCurrentTime() {
  const auto time = std::time(nullptr);
  Set(static_cast<detail::value_type>(time));
}

detail::value_type Gauge::Value() const { return value_; }

ClientMetric Gauge::Collect() const {
  ClientMetric metric;
  metric.gauge.value = Value();
  return metric;
}

}  // namespace prometheus

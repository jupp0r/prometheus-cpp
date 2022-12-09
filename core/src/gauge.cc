#include "prometheus/gauge.h"

#include <ctime>

namespace prometheus {

Gauge::Gauge(const double value) : value_{value} {}

void Gauge::Increment() { Increment(1.0); }

void Gauge::Increment(const double value) { Change(value); }

void Gauge::Decrement() { Decrement(1.0); }

void Gauge::Decrement(const double value) { Change(-1.0 * value); }

void Gauge::Set(const double value) { value_.store(value); }

void Gauge::Change(const double value) {
#if __cpp_lib_atomic_float >= 201711L
  value_.fetch_add(value);
#else
  // Pre-C++ 20 fallback: busy loop (which might be more expansive than using
  // fetch_add).
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value)) {
    // intentionally empty block
  }
#endif
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

}  // namespace prometheus

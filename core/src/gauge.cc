#include "prometheus/gauge.h"

#include <ctime>

namespace prometheus {

Gauge::Gauge(const double value) : value_{value} {}

void Gauge::Increment() { Increment(1.0); }

void Gauge::Increment(const double value) { Change(value); }

void Gauge::Decrement() { Decrement(1.0); }

void Gauge::Decrement(const double value) { Change(-1.0 * value); }

void Gauge::Set(const double value) {
  value_.store(value, std::memory_order_relaxed);
}

void Gauge::Change(const double value) {
#if __cpp_lib_atomic_float >= 201711L
  value_.fetch_add(value, std::memory_order_relaxed);
#else
  // Pre-C++ 20 fallback: busy loop (which might be more expansive than using
  // fetch_add).
  auto current = value_.load(std::memory_order_relaxed);
  while (!value_.compare_exchange_weak(current, current + value,
                                       std::memory_order_relaxed,
                                       std::memory_order_relaxed)) {
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

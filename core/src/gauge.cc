#include <prometheus/gauge.h>

#include <ctime>

namespace prometheus {
Gauge::Gauge() : value_{0} {}

Gauge::Gauge(double value) : value_{value} {}

void Gauge::Increment() { Increment(1.0); }
void Gauge::Increment(double value) {
  if (value < 0.0) {
    return;
  }
  Change(value);
}

void Gauge::Decrement() { Decrement(1.0); }

void Gauge::Decrement(double value) {
  if (value < 0.0) {
    return;
  }
  Change(-1.0 * value);
}

void Gauge::Set(double value) { value_.store(value); }

void Gauge::Change(double value) {
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value))
    ;
}

void Gauge::SetToCurrentTime() {
  auto time = std::time(nullptr);
  Set(static_cast<double>(time));
}

double Gauge::Value() const { return value_; }

ClientMetric Gauge::Collect() {
  ClientMetric metric;
  metric.gauge.value = Value();
  return metric;
}
}  // namespace prometheus

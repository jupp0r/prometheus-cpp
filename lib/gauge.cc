#include <ctime>

#include "gauge.h"

namespace prometheus {
Gauge::Gauge() : value_{0} {}

Gauge::Gauge(double value) : value_{value} {}

void Gauge::inc() { inc(1.0); }
void Gauge::inc(double value) {
  if (value < 0.0) {
    return;
  }
  change(value);
}

void Gauge::dec() { dec(1.0); }

void Gauge::dec(double value) {
  if (value < 0.0) {
    return;
  }
  change(-1.0 * value);
}

void Gauge::set(double value) { value_.store(value); }

void Gauge::change(double value) {
    auto current = value_.load();
    while (!value_.compare_exchange_weak(current, current + value))
        ;
}

void Gauge::set_to_current_time() {
  auto time = std::time(nullptr);
  set(static_cast<double>(time));
}

double Gauge::value() const { return value_; }
}

#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() {
  gauge_.Increment();
  UpdateTS();
}

void Counter::Increment(const double val) {
  if (val < 0.0) {
    return;
  }
  gauge_.Increment(val);
  UpdateTS();
}

double Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

}  // namespace prometheus

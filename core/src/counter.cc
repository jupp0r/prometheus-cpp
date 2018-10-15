#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() { gauge_.Increment(); }

void Counter::Increment(double val) { gauge_.Increment(val); }

double Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}
}  // namespace prometheus

#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() { gauge_.Increment(); }

void Counter::Increment(const double val) { gauge_.Increment(val); }

double Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

bool Counter::Expired(const std::time_t& time, const double& seconds) const {
  return false;
}

}  // namespace prometheus

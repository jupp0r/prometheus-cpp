#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment(const double& value, const bool& alert) {
  if (value < 0.0) return;
  value_ = value_ + value;
  last_update_ = std::time(nullptr);
  if (alert) AlertIfNoFamily();
}

double Counter::Value() const { return value_; }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

}  // namespace prometheus

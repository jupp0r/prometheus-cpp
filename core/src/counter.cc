#include "prometheus/counter.h"

namespace prometheus {


Counter::Counter(const bool alert_if_no_family) : MetricBase(alert_if_no_family) {};


void Counter::Increment(const double value) {
  if (value < 0.0) return;
  value_ = value_ + value;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

double Counter::Value() const { return value_; }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

}  // namespace prometheus

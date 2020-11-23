#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() { gauge_.Increment(); }

void Counter::Increment(const detail::value_type val) {
  if (val < 0.0) {
    return;
  }
  gauge_.Increment(val);
}

detail::value_type Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

}  // namespace prometheus

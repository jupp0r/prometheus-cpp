#include "prometheus/counter.h"

#include <numeric>

namespace prometheus {

double Counter::Value() const {
  return std::accumulate(
      std::begin(per_thread_counter_), std::end(per_thread_counter_), 0.0,
      [](const double a, const CacheLine& b) { return a + b.v; });
}

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

}  // namespace prometheus

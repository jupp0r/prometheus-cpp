#include "prometheus/counter.h"

#include "detail/builder.impl.h"
#include "family.impl.h"
#include "registry.impl.h"

namespace prometheus {

void Counter::Increment() { gauge_.Increment(); }

void Counter::Increment(const double val) { gauge_.Increment(val); }

double Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

template class PROMETHEUS_CPP_CORE_EXPORT detail::Builder<Counter>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Counter>;

template Family<Counter>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels);

detail::Builder<Counter> BuildCounter() { return {}; }

}  // namespace prometheus

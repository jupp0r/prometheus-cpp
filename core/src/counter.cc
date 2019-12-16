#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() {
  gauge_.Increment(); 
  last_update_.store(std::time(nullptr));
}

void Counter::Increment(const double val) {
  gauge_.Increment(val);
  last_update_.store(std::time(nullptr));
}

double Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

void Counter::UpdateRetentionTime(const double& retention_time, const bool& bump) {
  if (bump) last_update_.store(std::time(nullptr));
  retention_time_ = retention_time; 
};

bool Counter::Expired() const {
  return std::difftime(std::time(nullptr), last_update_) > retention_time_;
}

}  // namespace prometheus

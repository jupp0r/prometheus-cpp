#include "prometheus/metric_base.h"

#include <ctime>
#include <iostream>

namespace prometheus {

bool MetricBase::HasFamily() { return has_family_; }

void MetricBase::AlertIfNoFamily() {
  if (!has_family_) std::cerr << "prometheus-cpp:: This metric has no family" << std::endl;
}

void MetricBase::UpdateRetentionTime(const double& retention_time, const bool& bump) {
  if (bump) last_update_.store(std::time(nullptr));
  retention_time_.store(retention_time);
};

bool MetricBase::Expired() const {
  return std::difftime(std::time(nullptr), last_update_.load()) > retention_time_.load();
}

}  // namespace prometheus

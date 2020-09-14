#include "prometheus/metric_base.h"

#include <ctime>
#include <iostream>

namespace prometheus {

MetricBase::MetricBase(const bool alert_if_no_family) : alert_if_no_family_(alert_if_no_family) {}

bool MetricBase::HasFamily() { return has_family_; }

void MetricBase::EnableAlertIfNoFamily() { alert_if_no_family_ = true; }

void MetricBase::DisableAlertIfNoFamily() { alert_if_no_family_ = false; }

void MetricBase::AlertIfNoFamily() {
  if (alert_if_no_family_ && !has_family_) std::cerr << "prometheus-cpp:: This metric has no family (changes to the c++ object will not get propagated to prometheus)" << std::endl;
}

void MetricBase::UpdateRetentionTime(const double retention_time, const bool bump) {
  if (bump) last_update_.store(std::time(nullptr));
  retention_time_.store(retention_time);
};

bool MetricBase::IsExpired() const {
  return std::difftime(std::time(nullptr), last_update_) > retention_time_;
}

}  // namespace prometheus

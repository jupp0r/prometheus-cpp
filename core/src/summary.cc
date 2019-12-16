#include "prometheus/summary.h"

namespace prometheus {

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::milliseconds& max_age,
                 const int& age_buckets)
    : quantiles_(quantiles),
      count_(0),
      sum_(0),
      quantile_values_(quantiles_, max_age, age_buckets) {}

void Summary::Observe(const double value) {
  std::lock_guard<std::mutex> lock(mutex_);

  count_ += 1;
  sum_ += value;
  quantile_values_.insert(value);
  last_update_.store(std::time(nullptr));
}

ClientMetric Summary::Collect() {
  auto metric = ClientMetric{};

  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto& quantile : quantiles_) {
    auto metricQuantile = ClientMetric::Quantile{};
    metricQuantile.quantile = quantile.quantile;
    metricQuantile.value = quantile_values_.get(quantile.quantile);
    metric.summary.quantile.push_back(std::move(metricQuantile));
  }
  metric.summary.sample_count = count_;
  metric.summary.sample_sum = sum_;

  return metric;
}

void Summary::UpdateRetentionTime(const double& retention_time, const bool& bump) {
  if (bump) last_update_.store(std::time(nullptr));
  retention_time_ = retention_time; 
};

bool Summary::Expired() const {
  return std::difftime(std::time(nullptr), last_update_) > retention_time_;
}

}  // namespace prometheus

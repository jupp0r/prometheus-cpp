#include "prometheus/summary.h"

#include <utility>

namespace prometheus {

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::milliseconds max_age, const int age_buckets,
                 const double sample_ratio)
    : quantiles_{quantiles},
      quantile_values_{quantiles_, max_age, age_buckets} {
  if (sample_ratio > 0 && sample_ratio < 1) {
    sample_ratio_threshold_ = sample_ratio * RAND_MAX;
  }
}

Summary::Summary(Quantiles&& quantiles, const std::chrono::milliseconds max_age,
                 const int age_buckets, const double sample_ratio)
    : quantiles_{std::move(quantiles)},
      quantile_values_{quantiles_, max_age, age_buckets} {
  if (sample_ratio > 0 && sample_ratio < 1) {
    sample_ratio_threshold_ = sample_ratio * RAND_MAX;
  }
}

void Summary::Observe(const double value) {
  bool should_observe =
      (sample_ratio_threshold_ == 0 || sample_ratio_threshold_ < std::rand());
  std::lock_guard<std::mutex> lock(mutex_);

  count_ += 1;
  sum_ += value;
  if (should_observe) {
    quantile_values_.insert(value);
  }
}

ClientMetric Summary::Collect() const {
  auto metric = ClientMetric{};

  std::lock_guard<std::mutex> lock(mutex_);

  metric.summary.quantile.reserve(quantiles_.size());
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

}  // namespace prometheus

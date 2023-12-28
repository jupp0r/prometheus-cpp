#include "prometheus/summary.h"

#include <utility>

namespace prometheus {

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::milliseconds max_age, const int age_buckets)
    : quantiles_{quantiles},
      quantile_values_{quantiles_, max_age, age_buckets} {}

Summary::Summary(Quantiles&& quantiles, const std::chrono::milliseconds max_age,
                 const int age_buckets)
    : quantiles_{std::move(quantiles)},
      quantile_values_{quantiles_, max_age, age_buckets} {}

void Summary::Observe(const double value) {
  std::lock_guard<std::mutex> lock(mutex_);

  count_ += 1;
  sum_ += value;
  quantile_values_.insert(value);
}

SummaryMetric Summary::Collect() const {
  auto metric = SummaryMetric{};

  std::lock_guard<std::mutex> lock(mutex_);

  metric.quantile.reserve(quantiles_.size());
  for (const auto& quantile : quantiles_) {
    auto metricQuantile = SummaryMetric::Quantile{};
    metricQuantile.quantile = quantile.quantile;
    metricQuantile.value = quantile_values_.get(quantile.quantile);
    metric.quantile.push_back(std::move(metricQuantile));
  }
  metric.sample_count = count_;
  metric.sample_sum = sum_;

  return metric;
}

}  // namespace prometheus

#include "prometheus/summary.h"

#include "detail/builder.impl.h"
#include "family.impl.h"
#include "registry.impl.h"

namespace prometheus {

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::milliseconds max_age, const int age_buckets)
    : quantiles_{quantiles},
      count_{0},
      sum_{0},
      quantile_values_{quantiles_, max_age, age_buckets} {}

void Summary::Observe(const double value) {
  std::lock_guard<std::mutex> lock(mutex_);

  count_ += 1;
  sum_ += value;
  quantile_values_.insert(value);
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

template class PROMETHEUS_CPP_CORE_EXPORT Family<Summary>;
template class PROMETHEUS_CPP_CORE_EXPORT detail::Builder<Summary>;

template Family<Summary>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels);

detail::Builder<Summary> BuildSummary() { return {}; }

}  // namespace prometheus

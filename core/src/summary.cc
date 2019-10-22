#include "prometheus/summary.h"
#include "prometheus/registry.h"

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

template <>
Summary& Family<Summary>::WithLabelValues(const std::vector<std::string>& values) {
  return Add(VariableLabels(values), quantiles_);
}

namespace detail {
template<>
Family<Summary> &detail::Builder<Summary>::Register(Registry &registry) {
  Family<Summary> &family = registry.Add<Summary>(name_, help_, variable_labels_, labels_);
  return family.SetQuantiles(quantiles_);
}
} // namespace detail

}  // namespace prometheus

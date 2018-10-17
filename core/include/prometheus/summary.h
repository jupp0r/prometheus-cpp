#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/detail/quantiles.h"
#include "prometheus/metric_type.h"

namespace prometheus {

class Summary {
 public:
  using Quantiles = std::vector<detail::CKMSQuantiles::Quantile>;

  static const MetricType metric_type = MetricType::Summary;

  Summary(const Quantiles& quantiles,
          std::chrono::milliseconds max_age_seconds = std::chrono::seconds(60),
          int age_buckets = 5);

  void Observe(double value);

  ClientMetric Collect();

 private:
  const Quantiles quantiles_;

  std::mutex mutex_;

  double count_;
  double sum_;
  detail::TimeWindowQuantiles quantile_values_;
};
}  // namespace prometheus

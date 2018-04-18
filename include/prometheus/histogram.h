#pragma once

#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"

namespace prometheus {
class Histogram {
 public:
  using BucketBoundaries = std::vector<double>;

  static const MetricType metric_type = MetricType::Histogram;

  Histogram(const BucketBoundaries& buckets);

  void Observe(double value);

  ClientMetric Collect();

 private:
  const BucketBoundaries bucket_boundaries_;
  std::vector<Counter> bucket_counts_;
  Counter sum_;
};
}

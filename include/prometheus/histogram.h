#pragma once

#include <vector>

#include "prometheus/counter.h"

#include "metrics.pb.h"

namespace prometheus {
class Histogram : public Metric {
 public:
  using BucketBoundaries = std::vector<double>;

  static const io::prometheus::client::MetricType metric_type =
      io::prometheus::client::HISTOGRAM;

  Histogram(const BucketBoundaries& buckets);

  void Observe(double value);

  io::prometheus::client::Metric Collect();

 private:
  const BucketBoundaries bucket_boundaries_;
  std::vector<Counter> bucket_counts_;
  Counter sum_;
};
}

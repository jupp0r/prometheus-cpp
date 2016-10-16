#pragma once

#include <vector>

#include "cpp/metrics.pb.h"

#include "counter.h"

namespace prometheus {
class Histogram : public Metric {
 public:
  using BucketBoundaries = std::vector<double>;

  static const io::prometheus::client::MetricType metric_type =
      io::prometheus::client::HISTOGRAM;

  Histogram(const BucketBoundaries& buckets);

  void observe(double value);

  io::prometheus::client::Metric collect();

 private:
  const BucketBoundaries bucketBoundaries_;
  std::vector<Counter> bucketCounts_;
  Counter sum_;
};
}

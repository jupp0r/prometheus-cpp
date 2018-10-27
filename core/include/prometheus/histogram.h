#pragma once

#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/metric_type.h"

namespace prometheus {

/// \brief A histogram metric to represent aggregatable distributions of events.
///
/// This class represents the metric type histogram:
/// https://prometheus.io/docs/concepts/metric_types/#histogram
///
/// A histogram tracks the number of observations and the sum of the observed
/// values, allowing to calculate the average of the observed values.
///
/// At its core a histogram has a counter per bucket. The sum of observations
/// also behaves like a counter.
///
/// See https://prometheus.io/docs/practices/histograms/ for detailed
/// explanations of histogram usage and differences to summaries.
///
/// The class is thread-safe. No concurrent call to any API of this type causes a data race.
class Histogram {
 public:
  using BucketBoundaries = std::vector<double>;

  static const MetricType metric_type{MetricType::Histogram};

  /// \brief Create a histogram with manually choosen buckets.
  ///
  /// The BucketBoundaries are a list of monotonically increasing values
  /// representing the bucket boundaries. Each consecutive pair of values is
  /// interpreted as a half-open interval [b_n, b_n+1) which defines one bucket.
  ///
  /// There is no limitation on how the buckets are divided, i.e, equal size,
  /// exponential etc..
  ///
  /// The bucket boundaries cannot be changed once the histogram is created.
  Histogram(const BucketBoundaries& buckets);

  /// \brief Observe the given amount.
  ///
  /// The given amount selects the 'observed' bucket. The observed bucket is
  /// chosen for which the given amount falls into the half-open interval [b_n,
  /// b_n+1). The counter of the observed bucket is incremented. Also the total
  /// sum of all observations is incremented.
  void Observe(double value);

  ClientMetric Collect() const;

 private:
  const BucketBoundaries bucket_boundaries_;
  std::vector<Counter> bucket_counts_;
  Counter sum_;
};

}  // namespace prometheus

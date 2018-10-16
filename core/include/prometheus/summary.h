#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/detail/ckms_quantiles.h"
#include "prometheus/detail/time_window_quantiles.h"
#include "prometheus/metric_type.h"

namespace prometheus {

/// \brief A summary metric samples observations over a sliding window of time.
///
/// This class represents the metric type summary:
/// https://prometheus.io/docs/instrumenting/writing_clientlibs/#summary
///
/// A summary provides a total count of observations and a sum of all observed
/// values. In contrast to a histogram metric it also calculates configurable
/// Phi-quantiles over a sliding window of time.
///
/// The essential difference between summaries and histograms is that summaries
/// calculate streaming Phi-quantiles on the client side and expose them
/// directly, while histograms expose bucketed observation counts and the
/// calculation of quantiles from the buckets of a histogram happens on the
/// server side:
/// https://prometheus.io/docs/prometheus/latest/querying/functions/#histogram_quantile.
///
/// Note that Phi designates the probability density function of the standard
/// Gaussian distribution.
///
/// See https://prometheus.io/docs/practices/histograms/ for detailed
/// explanations of Phi-quantiles, summary usage, and differences to histograms.
class Summary {
 public:
  using Quantiles = std::vector<detail::CKMSQuantiles::Quantile>;

  static const MetricType metric_type = MetricType::Summary;

  /// \brief Create a summary metric.
  ///
  /// \param quantiles A list of 'targeted' Phi-quantiles. A targeted
  /// Phi-quantile is specified in the form of a Phi-quantile and tolerated
  /// error. For example a Quantile{0.5, 0.1} means that the median (= 50th
  /// percentile) should be returned with 10 percent error or a Quantile{0.2,
  /// 0.05} means the 20th percentile with 5 percent tolerated error. Note that
  /// percentiles and quantiles are the same concept, except percentiles are
  /// expressed as percentages. The Phi-quantile must be in the interval [0, 1].
  /// Note that a lower tolerated error for a Phi-quantile results in higher
  /// usage of resources (memory and cpu) to calculate the summary.
  ///
  /// The Phi-quantiles are calculated over a sliding window of time. The
  /// sliding window of time is configured by max_age_seconds and age_buckets.
  ///
  /// \param max_age_seconds Set the duration of the time window, i.e., how long
  /// observations are kept before they are discarded. The default value is 60
  /// seconds.
  ///
  /// \param age_buckets Set the number of buckets of the time window. It
  /// determines the number of buckets used to exclude observations that
  /// are older than max_age_seconds from the summary, e.g., if max_age_seconds
  /// is 60 seconds and age_buckets is 5, buckets will be switched every 12
  /// seconds. The value is a trade-off between resources (memory and cpu for
  /// maintaining the bucket) and how smooth the time window is moved. With only
  /// one age bucket it effectively results in a complete reset of the summary
  /// each time max_age_seconds has passed. The default value is 5.
  Summary(const Quantiles& quantiles,
          std::chrono::milliseconds max_age_seconds = std::chrono::seconds(60),
          int age_buckets = 5);

  /// \brief Observe the given amount.
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

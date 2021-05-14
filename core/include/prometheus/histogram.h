#pragma once

#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/detail/builder.h"  // IWYU pragma: export
#include "prometheus/detail/core_export.h"
#include "prometheus/gauge.h"
#include "prometheus/metric_type.h"
#include "prometheus/family.h"

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
/// also behaves like a counter as long as there are no negative observations.
///
/// See https://prometheus.io/docs/practices/histograms/ for detailed
/// explanations of histogram usage and differences to summaries.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMETHEUS_CPP_CORE_EXPORT Histogram {
 public:

  /// \brief ExponentialBuckets creates 'count' buckets, where the lowest bucket has an
  /// upper bound of 'start' and each following bucket's upper bound is 'factor'
  /// times the previous bucket's upper bound. The final +Inf bucket is not counted
  /// and not included in the returned vector. The returned vector is meant to be
  /// used for the Buckets field of Histogram.
  ///
  /// The function assert if 'count' is 0 or negative, if 'start' is 0 or negative,
  /// or if 'factor' is less than or equal 1.
  static std::vector<double> ExponentialBuckets(double start,
         double factor, int count);
 public:
  using BucketBoundaries = std::vector<double>;

  static const MetricType metric_type{MetricType::Histogram};

  /// \brief Create a histogram with manually chosen buckets.
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

  /// \brief Observe multiple data points.
  ///
  /// Increments counters given a count for each bucket. (i.e. the caller of
  /// this function must have already sorted the values into buckets).
  /// Also increments the total sum of all observations by the given value.
  void ObserveMultiple(const std::vector<double>& bucket_increments,
                       const double sum_of_values);

  /// \brief Get the current value of the counter.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

 private:
  const BucketBoundaries bucket_boundaries_;
  std::vector<Counter> bucket_counts_;
  Gauge sum_;
};

/// \brief Return a builder to configure and register a Histogram metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& histogram_family = prometheus::BuildHistogram()
///                              .Name("some_name")
///                              .Help("Additional description.")
///                              .Labels({{"key", "value"}})
///                              .Register(*registry);
///
/// histogram_family.Add({{"key1","value1"}}, Histogram::BucketBoundaries{1, 2}).Observe(1.0);
/// ...
/// \endcode
///
/// Example usage2:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& histogram_family = prometheus::BuildHistogram()
///                              .Name("some_name")
///                              .Help("Additional description.")
///                              .Labels({{"key", "value"}})
///                              .LabelNamesVec({"key2","key3"})
///                              .BucketBoundaries({Histogram::BucketBoundaries{1, 2}})
///                              .Register(*registry);
///
/// histogram_family.WithLabelValues({"value2","value3"}).Observe(1.0);
/// ...
/// \endcode
///
/// \return An object of unspecified type T, i.e., an implementation detail
/// except that it has the following members:
///
/// - Name(const std::string&) to set the metric name,
/// - Help(const std::string&) to set an additional description.
/// - Label(const std::map<std::string, std::string>&) to assign a set of
///   key-value pairs (= labels) to the metric.
/// - LabelNamesVec(const std::vector<std::string&) to pre-affirmation pairs(= labels)'s
///   key; and you and use family.WithLabelValues({"value1","value1"}) to get the T;
///   note than: vector<names>.size() == vector<values>.size()
/// - BucketBoundaries(const std::vector<double>&) to pre-affirmation bucketBoundaries
///   when use WithLabelValues()
///
/// To finish the configuration of the Histogram metric register it with
/// Register(Registry&).
PROMETHEUS_CPP_CORE_EXPORT detail::Builder<Histogram> BuildHistogram();

/// \brief Specialization of WithLabelValues<Histogram>.
PROMETHEUS_CPP_CORE_EXPORT template <>
Histogram& Family<Histogram>::WithLabelValues(const std::vector<std::string>& values);

namespace detail {
/// \brief Specialization of Register<Histogram>.
PROMETHEUS_CPP_CORE_EXPORT template<>
Family <Histogram> &Builder<Histogram>::Register(Registry &registry);
}  // namespace detail

}  // namespace prometheus

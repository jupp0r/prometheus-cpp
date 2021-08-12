#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/detail/builder.h"  // IWYU pragma: export
#include "prometheus/detail/core_export.h"
#include "prometheus/metric.h"
#include "prometheus/metric_type.h"

namespace prometheus {

/// \brief A summary metric whose quantiles are specified manually.
///
/// This class represents the metric type summary:
/// https://prometheus.io/docs/instrumenting/writing_clientlibs/#summary
///
/// A manual summary provides a total count of observations and a sum of all
/// observed values. In contrast to a common summary metric its quantiles are
/// specified manually.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMETHEUS_CPP_CORE_EXPORT ManualSummary : public Metric {
 public:
  using ManualQuantiles = std::map<double, double>;

  static const MetricType metric_type{MetricType::ManualSummary};

  /// \brief Create a manual summary metric.
  ManualSummary() = default;

  /// \brief Set count.
  void SetCount(std::uint64_t count);

  /// \brief Set sum.
  void SetSum(double sum);

  /// \brief Add quantile.
  void AddQuantile(double quantile, double value);

  /// \brief Get the current value of the summary.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

 private:
  ManualQuantiles quantiles_;
  mutable std::mutex mutex_;
  std::uint64_t count_ = 0;
  double sum_ = 0;
};

/// \brief Return a builder to configure and register a ManualSummary metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& manual_summary_family = prometheus::ManualSummary()
///                            .Name("some_name")
///                            .Help("Additional description.")
///                            .Labels({{"key", "value"}})
///                            .Register(*registry);
///
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
///
/// To finish the configuration of the ManualSummary metric register it with
/// Register(Registry&).
PROMETHEUS_CPP_CORE_EXPORT detail::Builder<ManualSummary> BuildManualSummary();

}  // namespace prometheus

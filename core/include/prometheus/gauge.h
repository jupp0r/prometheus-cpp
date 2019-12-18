#pragma once

#include <atomic>
#include <ctime>

#include "prometheus/client_metric.h"
#include "prometheus/detail/builder.h"
#include "prometheus/detail/core_export.h"
#include "prometheus/metric_type.h"
#include "prometheus/metric_base.h"

namespace prometheus {

/// \brief A gauge metric to represent a value that can arbitrarily go up and
/// down.
///
/// The class represents the metric type gauge:
/// https://prometheus.io/docs/concepts/metric_types/#gauge
///
/// Gauges are typically used for measured values like temperatures or current
/// memory usage, but also "counts" that can go up and down, like the number of
/// running processes.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMETHEUS_CPP_CORE_EXPORT Gauge: public MetricBase {
 public:
  static const MetricType metric_type{MetricType::Gauge};

  /// \brief Create a gauge that starts at the given amount.
  Gauge(const double& value = 0);

  /// \brief Increment the gauge by the given amount.
  void Increment(const double& value = 1, const bool& alert = true);

  /// \brief Decrement the gauge by the given amount.
  void Decrement(const double& value = 1, const bool& alert = true);

  /// \brief Set the gauge to the given value.
  void Set(const double& value, const bool& alert = true);

  /// \brief Set the gauge to the current unixtime in seconds.
  void SetToCurrentTime();

  /// \brief Get the current value of the gauge.
  double Value() const;

  /// \brief Get the current value of the gauge.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const; 

 private:
  std::atomic<double> value_{0.0};
};

/// \brief Return a builder to configure and register a Gauge metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& gauge_family = prometheus::BuildGauge()
///                          .Name("some_name")
///                          .Help("Additional description.")
///                          .Labels({{"key", "value"}})
///                          .Register(*registry);
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
/// To finish the configuration of the Gauge metric register it with
/// Register(Registry&).
PROMETHEUS_CPP_CORE_EXPORT detail::Builder<Gauge> BuildGauge();

}  // namespace prometheus

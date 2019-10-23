#pragma once

#include "prometheus/client_metric.h"
#include "prometheus/detail/builder.h"
#include "prometheus/detail/core_export.h"
#include "prometheus/gauge.h"
#include "prometheus/metric_type.h"
#include "prometheus/family.h"

namespace prometheus {

/// \brief A counter metric to represent a monotonically increasing value.
///
/// This class represents the metric type counter:
/// https://prometheus.io/docs/concepts/metric_types/#counter
///
/// The value of the counter can only increase. Example of counters are:
/// - the number of requests served
/// - tasks completed
/// - errors
///
/// Do not use a counter to expose a value that can decrease - instead use a
/// Gauge.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMETHEUS_CPP_CORE_EXPORT Counter {
 public:
  static const MetricType metric_type{MetricType::Counter};

  /// \brief Create a counter that starts at 0.
  Counter() = default;

  /// \brief Increment the counter by 1.
  void Increment();

  /// \brief Increment the counter by a given amount.
  ///
  /// The counter will not change if the given amount is negative.
  void Increment(double);

  /// \brief Get the current value of the counter.
  double Value() const;

  /// \brief Get the current value of the counter.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

 private:
  Gauge gauge_{0.0};
};

/// \brief Return a builder to configure and register a Counter metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& counter_family = prometheus::BuildCounter()
///                            .Name("some_name")
///                            .Help("Additional description.")
///                            .Labels({{"key", "value"}})
///                            .Register(*registry);
///
/// counter_family.Add({{"key2","value2"}}).Increment();
/// ...
/// \endcode
///
/// Example usage2:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& counter_family = prometheus::BuildCounter()
///                            .Name("some_name")
///                            .Help("Additional description.")
///                            .Labels({{"key", "value"}})
///                            .LabelNamesVec({"key2","key3"})
///                            .Register(*registry);
///
/// counter_family.WithLabelValues({"value2","value3"}).Increment();
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
///
/// To finish the configuration of the Counter metric, register it with
/// Register(Registry&).
PROMETHEUS_CPP_CORE_EXPORT detail::Builder<Counter> BuildCounter();

/// \brief Specialization of WithLabelValues<Counter>.
PROMETHEUS_CPP_CORE_EXPORT template <>
Counter& Family<Counter>::WithLabelValues(const std::vector<std::string>& values);

}  // namespace prometheus

#pragma once

#include "prometheus/client_metric.h"
#include "prometheus/gauge.h"
#include "prometheus/metric_type.h"

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
class Counter {
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

}  // namespace prometheus

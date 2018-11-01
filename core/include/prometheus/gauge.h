#pragma once

#include <atomic>

#include "prometheus/client_metric.h"
#include "prometheus/metric_type.h"

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
class Gauge {
 public:
  static const MetricType metric_type{MetricType::Gauge};

  /// \brief Create a gauge that starts at 0.
  Gauge() = default;

  /// \brief Create a gauge that starts at the given amount.
  Gauge(double);

  /// \brief Increment the gauge by 1.
  void Increment();

  /// \brief Increment the gauge by the given amount.
  void Increment(double);

  /// \brief Decrement the gauge by 1.
  void Decrement();

  /// \brief Decrement the gauge by the given amount.
  void Decrement(double);

  /// \brief Set the gauge to the given value.
  void Set(double);

  /// \brief Set the gauge to the current unixtime in seconds.
  void SetToCurrentTime();

  /// \brief Get the current value of the gauge.
  double Value() const;

  /// \brief Get the current value of the gauge.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

 private:
  void Change(double);
  std::atomic<double> value_{0.0};
};

}  // namespace prometheus

#pragma once

#include <array>
#include <atomic>
#include <exception>
#include <thread>

#include "prometheus/client_metric.h"
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
/// Gauge. If an montonically increasing counter is applicable a counter shall
/// be prefered to a Gauge because of a better update performance.
///
/// The implementation exhibits a performance which is near a sequential
/// implementation and scales linearly with increasing number of updater threads
/// in a multi-threaded environment invoking Increment(). However, this
/// excellent update-side scalability comes at read-side expense invoking
/// Collect(). Increment() can therefor be used in the fast-path of the code,
/// where the count is updated extremely frequently. The Collect() function on
/// the other hand shall read the counter at a low sample rate, e.g., in the
/// order of milliseconds.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class Counter {
 public:
  static const MetricType metric_type{MetricType::Counter};

  /// \brief Create a counter that starts at 0.
  Counter() = default;

  /// \brief Increment the counter by 1.
  void Increment() { IncrementUnchecked(1.0); }

  /// \brief Increment the counter by a given amount.
  ///
  /// The counter will not change if the given amount is negative.
  void Increment(const double value) {
    if (value < 0.0) {
      return;
    }
    IncrementUnchecked(value);
  }

  /// \brief Get the current value of the counter.
  double Value() const;

  /// \brief Get the current value of the counter.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

 private:
  int BucketId() {
    thread_local int id{-1};

    if (id == -1) {
      id = AssignBucketId();
    }
    return id;
  }

  int AssignBucketId() {
    const auto threadIdHash = std::hash<std::thread::id>{}(std::this_thread::get_id());
    return threadIdHash % per_thread_counter_.size();
  }

  void IncrementUnchecked(const double v) {
    CacheLine& c = per_thread_counter_[BucketId()];
    const double new_value{c.v.load(std::memory_order_relaxed) + v};
    c.v.store(new_value, std::memory_order_relaxed);
  }

  struct alignas(128) CacheLine {
    std::atomic<double> v{0.0};
  };

  std::array<CacheLine, 128> per_thread_counter_{};
};

}  // namespace prometheus

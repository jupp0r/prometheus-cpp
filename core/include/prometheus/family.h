#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "prometheus/check_names.h"
#include "prometheus/client_metric.h"
#include "prometheus/collectable.h"
#include "prometheus/metric_family.h"

namespace prometheus {

/// \brief A metric of type T with a set of time series.
///
/// Every time series is uniquely identified by its metric name and a set of
/// key-value pairs, also known as labels. It is required to follow the syntax
/// of metric names and labels given by:
/// https://prometheus.io/docs/concepts/data_model/#metric-names-and-labels
///
/// The following metric and label conventions are not required for using
/// Prometheus, but can serve as both a style-guide and a collection of best
/// practices: https://prometheus.io/docs/practices/naming/
///
/// \tparam T One of the metric types Counter, Gauge, Histogram or Summary.
template <typename T>
class Family : public Collectable {
 public:
  /// \brief Create a new metric.
  ///
  /// Every metric is uniquely identified by its name and a set of key-value
  /// pairs, also known as labels.
  ///
  /// The example selects all time series that have the `http_requests_total`
  /// metric name:
  ///
  ///     http_requests_total
  ///
  /// It is possible to filter these time series further by appending a set of
  /// labels to match in curly braces ({})
  ///
  ///     http_requests_total{job="prometheus",group="canary"}
  ///
  /// For further information see: [Quering Basics]
  /// (https://prometheus.io/docs/prometheus/latest/querying/basics/)
  ///
  /// \param name Set the metric name.
  /// \param help Set an additional description.
  /// \param labels Configure a set of key-value pairs (= labels) attached to
  /// the metric. All these labels are automatically propagated to each time
  /// series within the metric.
  Family(const std::string& name, const std::string& help,
         const std::map<std::string, std::string>& labels);

  /// \brief Add a new time series.
  ///
  /// It is possible to filter the time series further by appending a set of
  /// labels to match in curly braces ({})
  ///
  ///     http_requests_total{job="prometheus",group="canary",method="GET"}
  ///
  /// \param labels Configure a set of key-value pairs (= labels) of the time
  /// series.
  /// \param args Arguments are passed to the constructor of metric type
  /// T. See Counter, Gauge, Histogram or Summary for required constructor
  /// arguments.
  template <typename... Args>
  T& Add(const std::map<std::string, std::string>& labels, Args&&... args);

  /// \brief Remove the given time series which was previously added with Add().
  ///
  /// \param metric Time series to be removed. The function does nothing, if the
  /// given time series is not part of the metric.
  void Remove(T* metric);

  std::vector<MetricFamily> Collect() override;

 private:
  std::unordered_map<std::size_t, std::unique_ptr<T>> metrics_;
  std::unordered_map<std::size_t, std::map<std::string, std::string>> labels_;
  std::unordered_map<T*, std::size_t> labels_reverse_lookup_;

  const std::string name_;
  const std::string help_;
  const std::map<std::string, std::string> constant_labels_;
  std::mutex mutex_;

  ClientMetric CollectMetric(std::size_t hash, T* metric);

  static std::size_t hash_labels(
      const std::map<std::string, std::string>& labels);
};

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const std::map<std::string, std::string>& labels)
    : name_(name), help_(help), constant_labels_(labels) {
  assert(CheckMetricName(name_));
}

template <typename T>
template <typename... Args>
T& Family<T>::Add(const std::map<std::string, std::string>& labels,
                  Args&&... args) {
#ifndef NDEBUG
  for (auto& label_pair : labels) {
    auto& label_name = label_pair.first;
    assert(CheckLabelName(label_name));
  }
#endif

  auto hash = hash_labels(labels);
  std::lock_guard<std::mutex> lock{mutex_};
  auto metrics_iter = metrics_.find(hash);

  if (metrics_iter != metrics_.end()) {
#ifndef NDEBUG
    auto labels_iter = labels_.find(hash);
    assert(labels_iter != labels_.end());
    const auto& old_labels = labels_iter->second;
    assert(labels == old_labels);
#endif
    return *metrics_iter->second;
  } else {
    auto metric = new T(std::forward<Args>(args)...);
    metrics_.insert(std::make_pair(hash, std::unique_ptr<T>{metric}));
    labels_.insert({hash, labels});
    labels_reverse_lookup_.insert({metric, hash});
    return *metric;
  }
}

template <typename T>
std::size_t Family<T>::hash_labels(
    const std::map<std::string, std::string>& labels) {
  auto combined = std::accumulate(
      labels.begin(), labels.end(), std::string{},
      [](const std::string& acc,
         const std::pair<std::string, std::string>& label_pair) {
        return acc + label_pair.first + label_pair.second;
      });
  return std::hash<std::string>{}(combined);
}

template <typename T>
void Family<T>::Remove(T* metric) {
  std::lock_guard<std::mutex> lock{mutex_};
  if (labels_reverse_lookup_.count(metric) == 0) {
    return;
  }

  auto hash = labels_reverse_lookup_.at(metric);
  metrics_.erase(hash);
  labels_.erase(hash);
  labels_reverse_lookup_.erase(metric);
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect() {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = MetricFamily{};
  family.name = name_;
  family.help = help_;
  family.type = T::metric_type;
  for (const auto& m : metrics_) {
    family.metric.push_back(std::move(CollectMetric(m.first, m.second.get())));
  }
  return {family};
}

template <typename T>
ClientMetric Family<T>::CollectMetric(std::size_t hash, T* metric) {
  auto collected = metric->Collect();
  auto add_label =
      [&collected](const std::pair<std::string, std::string>& label_pair) {
        auto label = ClientMetric::Label{};
        label.name = label_pair.first;
        label.value = label_pair.second;
        collected.label.push_back(std::move(label));
      };
  std::for_each(constant_labels_.cbegin(), constant_labels_.cend(), add_label);
  const auto& metric_labels = labels_.at(hash);
  std::for_each(metric_labels.cbegin(), metric_labels.cend(), add_label);
  return collected;
}

}  // namespace prometheus

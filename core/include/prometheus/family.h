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
  /// \param time_series Time series to be removed. The function does nothing,
  /// if the given time series is not part of the metric.
  void Remove(T* time_series);

  std::vector<MetricFamily> Collect() override;

 private:
  static std::size_t HashLabels(
      const std::map<std::string, std::string>& labels);

  ClientMetric CollectMetric(std::size_t hash, T* time_series);

  const std::string name_;
  const std::string help_;
  const std::map<std::string, std::string> metric_labels_;

  std::unordered_map<std::size_t, std::unique_ptr<T>> time_series_;
  std::unordered_map<std::size_t, std::map<std::string, std::string>>
      time_series_labels_;
  std::unordered_map<T*, std::size_t> time_series_labels_reverse_lookup_;

  std::mutex mutex_;
};

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const std::map<std::string, std::string>& labels)
    : name_(name), help_(help), metric_labels_(labels) {
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

  auto hash = HashLabels(labels);
  std::lock_guard<std::mutex> lock{mutex_};
  auto time_series_iter = time_series_.find(hash);

  if (time_series_iter != time_series_.end()) {
#ifndef NDEBUG
    auto time_series_labels_iter = time_series_labels_.find(hash);
    assert(time_series_labels_iter != time_series_labels_.end());
    const auto& old_labels = time_series_labels_iter->second;
    assert(labels == old_labels);
#endif
    return *time_series_iter->second;
  } else {
    auto time_series = new T(std::forward<Args>(args)...);
    time_series_.insert(std::make_pair(hash, std::unique_ptr<T>{time_series}));
    time_series_labels_.insert({hash, labels});
    time_series_labels_reverse_lookup_.insert({time_series, hash});
    return *time_series;
  }
}

template <typename T>
std::size_t Family<T>::HashLabels(
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
void Family<T>::Remove(T* time_series) {
  std::lock_guard<std::mutex> lock{mutex_};
  if (time_series_labels_reverse_lookup_.count(time_series) == 0) {
    return;
  }

  auto hash = time_series_labels_reverse_lookup_.at(time_series);
  time_series_.erase(hash);
  time_series_labels_.erase(hash);
  time_series_labels_reverse_lookup_.erase(time_series);
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect() {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = MetricFamily{};
  family.name = name_;
  family.help = help_;
  family.type = T::metric_type;
  for (const auto& t : time_series_) {
    family.metric.push_back(std::move(CollectMetric(t.first, t.second.get())));
  }
  return {family};
}

template <typename T>
ClientMetric Family<T>::CollectMetric(std::size_t hash, T* time_series) {
  auto collected = time_series->Collect();
  auto add_label =
      [&collected](const std::pair<std::string, std::string>& label_pair) {
        auto label = ClientMetric::Label{};
        label.name = label_pair.first;
        label.value = label_pair.second;
        collected.label.push_back(std::move(label));
      };
  std::for_each(metric_labels_.cbegin(), metric_labels_.cend(), add_label);
  const auto& time_series_labels = time_series_labels_.at(hash);
  std::for_each(time_series_labels.cbegin(), time_series_labels.cend(),
                add_label);
  return collected;
}

}  // namespace prometheus

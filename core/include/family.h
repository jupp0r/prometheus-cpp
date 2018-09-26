#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <unordered_map>

#include "prometheus/check_names.h"
#include "prometheus/collectable.h"
#include "prometheus/counter_builder.h"
#include "prometheus/gauge_builder.h"
#include "prometheus/histogram_builder.h"
#include "prometheus/metric.h"
#include "prometheus/metric_family.h"

namespace prometheus {

template <typename T>
class Family : public Collectable {
 public:
  friend class detail::CounterBuilder;
  friend class detail::GaugeBuilder;
  friend class detail::HistogramBuilder;

  Family(const std::string& name, const std::string& help,
         const std::map<std::string, std::string>& constant_labels);
  template <typename... Args>
  T& Add(const std::map<std::string, std::string>& labels, Args&&... args);
  void Remove(T* metric);

  // Collectable
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
                  const std::map<std::string, std::string>& constant_labels)
    : name_(name), help_(help), constant_labels_(constant_labels) {
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
    const auto &old_labels = labels_iter->second;
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
}

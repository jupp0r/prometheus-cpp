#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <numeric>
#include <string>
#include <unordered_map>

#include "check_names.h"
#include "collectable.h"
#include "counter_builder.h"
#include "gauge_builder.h"
#include "histogram_builder.h"
#include "metric.h"

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
  std::vector<io::prometheus::client::MetricFamily> Collect() override;

 private:
  std::unordered_map<std::size_t, std::unique_ptr<T>> metrics_;
  std::unordered_map<std::size_t, std::map<std::string, std::string>> labels_;
  std::unordered_map<T*, std::size_t> labels_reverse_lookup_;

  const std::string name_;
  const std::string help_;
  const std::map<std::string, std::string> constant_labels_;
  std::mutex mutex_;

  io::prometheus::client::Metric CollectMetric(std::size_t hash, T* metric);

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

  std::lock_guard<std::mutex> lock{mutex_};
  auto hash = hash_labels(labels);
  auto metric = new T(std::forward<Args>(args)...);

  metrics_.insert(std::make_pair(hash, std::unique_ptr<T>{metric}));
  labels_.insert({hash, labels});
  labels_reverse_lookup_.insert({metric, hash});
  return *metric;
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
std::vector<io::prometheus::client::MetricFamily> Family<T>::Collect() {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = io::prometheus::client::MetricFamily{};
  family.set_name(name_);
  family.set_help(help_);
  family.set_type(T::metric_type);
  for (const auto& m : metrics_) {
    *family.add_metric() = std::move(CollectMetric(m.first, m.second.get()));
  }
  return {family};
}

template <typename T>
io::prometheus::client::Metric Family<T>::CollectMetric(std::size_t hash,
                                                        T* metric) {
  auto collected = metric->Collect();
  auto add_label =
      [&collected](const std::pair<std::string, std::string>& label_pair) {
        auto pair = collected.add_label();
        pair->set_name(label_pair.first);
        pair->set_value(label_pair.second);
      };
  std::for_each(constant_labels_.cbegin(), constant_labels_.cend(), add_label);
  const auto& metric_labels = labels_.at(hash);
  std::for_each(metric_labels.cbegin(), metric_labels.cend(), add_label);
  return collected;
}
}

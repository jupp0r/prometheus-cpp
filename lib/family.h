#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>

#include "collectable.h"
#include "metric.h"

namespace prometheus {

template <typename T>
class Family : public Collectable {
 public:
  Family(const std::string& name, const std::string& help,
         const std::map<std::string, std::string>& constantLabels);
  T* add(const std::map<std::string, std::string>& labels);
  void remove(T* metric);

  // Collectable
  std::vector<io::prometheus::client::MetricFamily> collect() override;

 private:
  std::unordered_map<std::size_t, std::unique_ptr<T>> metrics_;
  std::unordered_map<std::size_t, std::map<std::string, std::string>> labels_;
  std::unordered_map<T*, std::size_t> labels_reverse_lookup_;

  const std::string name_;
  const std::string help_;
  const std::map<std::string, std::string> constantLabels_;

  io::prometheus::client::Metric collect_metric(std::size_t hash, T* metric);

  static std::size_t hash_labels(
      const std::map<std::string, std::string>& labels);
};

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const std::map<std::string, std::string>& constantLabels)
    : name_(name), help_(help), constantLabels_(constantLabels) {}

template <typename T>
T* Family<T>::add(const std::map<std::string, std::string>& labels) {
  auto hash = hash_labels(labels);
  auto metric = new T();

  metrics_.insert(std::make_pair(hash, std::unique_ptr<T>{metric}));
  labels_.insert({hash, labels});
  labels_reverse_lookup_.insert({metric, hash});
  return metric;
}

template <typename T>
std::size_t Family<T>::hash_labels(
    const std::map<std::string, std::string>& labels) {
  auto combined =
      std::accumulate(labels.begin(), labels.end(), std::string{},
                      [](const std::string& acc,
                         const std::pair<std::string, std::string>& labelPair) {
                        return acc + labelPair.first + labelPair.second;
                      });
  return std::hash<std::string>{}(combined);
}

template <typename T>
void Family<T>::remove(T* metric) {
  if (labels_reverse_lookup_.count(metric) == 0) {
    return;
  }

  auto hash = labels_reverse_lookup_.at(metric);
  metrics_.erase(hash);
  labels_.erase(hash);
  labels_reverse_lookup_.erase(metric);
}

template <typename T>
std::vector<io::prometheus::client::MetricFamily> Family<T>::collect() {
  auto family = io::prometheus::client::MetricFamily{};
  family.set_name(name_);
  family.set_help(help_);
  family.set_type(T::metric_type);
  for (const auto& m : metrics_) {
    *family.add_metric() = std::move(collect_metric(m.first, m.second.get()));
  }
  return {family};
}

template <typename T>
io::prometheus::client::Metric Family<T>::collect_metric(std::size_t hash,
                                                         T* metric) {
  auto collected = metric->collect();
  auto addLabel =
      [&collected](const std::pair<std::string, std::string>& labelPair) {
        auto pair = collected.add_label();
        pair->set_name(labelPair.first);
        pair->set_value(labelPair.second);
      };
  std::for_each(constantLabels_.cbegin(), constantLabels_.cend(), addLabel);
  const auto& metricLabels = labels_.at(hash);
  std::for_each(metricLabels.cbegin(), metricLabels.cend(), addLabel);
  return collected;
}
}

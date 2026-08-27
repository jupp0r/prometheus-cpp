#include "prometheus/family.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <utility>

#include "prometheus/check_names.h"
#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/info.h"
#include "prometheus/summary.h"

namespace prometheus {

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const Labels& constant_labels)
    : name_(name), help_(help), constant_labels_(constant_labels) {
  if (!CheckMetricName(name_)) {
    throw std::invalid_argument("Invalid metric name");
  }
  for (auto& [label_name, label_value] : constant_labels_) {
    if (!CheckLabelName(label_name, T::metric_type)) {
      throw std::invalid_argument("Invalid label name");
    }
  }
}

template <typename T>
T& Family<T>::Add(const Labels& labels, std::unique_ptr<T> object) {
  std::lock_guard<std::mutex> lock{mutex_};

  auto [it, inserted] = metrics_.emplace(labels, std::move(object));

  if (inserted) {
    // insertion took place, retroactively check for unlikely issues
    for (auto& [label_name, label_value] : labels) {
      if (!CheckLabelName(label_name, T::metric_type)) {
        metrics_.erase(it);
        throw std::invalid_argument("Invalid label name");
      }
      if (constant_labels_.count(label_name)) {
        metrics_.erase(it);
        throw std::invalid_argument("Duplicate label name");
      }
    }
  }

  auto& stored_object = it->second;
  assert(stored_object);
  return *stored_object;
}

template <typename T>
void Family<T>::Remove(T* metric) {
  std::lock_guard<std::mutex> lock{mutex_};

  const auto it = std::find_if(
      metrics_.begin(), metrics_.end(),
      [metric](const auto& entry) { return entry.second.get() == metric; });
  if (it != metrics_.end()) {
    metrics_.erase(it);
  }
}

template <typename T>
bool Family<T>::Has(const Labels& labels) const {
  std::lock_guard<std::mutex> lock{mutex_};
  return metrics_.count(labels) != 0u;
}

template <typename T>
const std::string& Family<T>::GetName() const {
  return name_;
}

template <typename T>
const Labels& Family<T>::GetConstantLabels() const {
  return constant_labels_;
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect() const {
  std::lock_guard<std::mutex> lock{mutex_};

  auto families = std::vector<MetricFamily>{};
  if (metrics_.empty()) {
    return families;
  }

  auto family = MetricFamily{};
  family.name = name_;
  family.help = help_;
  family.type = T::metric_type;
  family.metric.reserve(metrics_.size());
  for (const auto& [metric_labels, metric] : metrics_) {
    family.metric.push_back(CollectMetric(metric_labels, metric.get()));
  }
  families.push_back(std::move(family));
  return families;
}

template <typename T>
ClientMetric Family<T>::CollectMetric(const Labels& metric_labels,
                                      T* metric) const {
  auto collected = metric->Collect();
  collected.label.reserve(constant_labels_.size() + metric_labels.size());
  const auto add_label =
      [&collected](const std::pair<std::string, std::string>& label_pair) {
        auto label = ClientMetric::Label{};
        label.name = label_pair.first;
        label.value = label_pair.second;
        collected.label.push_back(std::move(label));
      };
  std::for_each(constant_labels_.cbegin(), constant_labels_.cend(), add_label);
  std::for_each(metric_labels.cbegin(), metric_labels.cend(), add_label);
  return collected;
}

template class PROMETHEUS_CPP_CORE_EXPORT Family<Counter>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Gauge>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Histogram>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Info>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Summary>;

}  // namespace prometheus

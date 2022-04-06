#include "prometheus/family.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "prometheus/check_names.h"
#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

template <class Key, class T, class Compare, class Alloc, class Pred>
void erase_if(std::map<Key, T, Compare, Alloc>& c, Pred pred) {
  for (auto i = c.begin(), last = c.end(); i != last;)
    if (pred(*i)) {
      i = c.erase(i);
    } else {
      ++i;
    }
}

auto empty_label_value = [](const Labels::value_type& label) {
  return label.second.empty();
};

// A label with an empty label value is considered equivalent to a label that
// does not exist.
void filter_labels(Labels& labels) {
  // with C++20 use std::erase_if
  erase_if(labels, empty_label_value);
}

Labels filter_and_return_labels(Labels labels) {
  filter_labels(labels);
  return labels;
}

}  // namespace

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  Labels constant_labels)
    : name_(name),
      help_(help),
      constant_labels_(filter_and_return_labels(std::move(constant_labels))) {
  if (!CheckMetricName(name_)) {
    throw std::invalid_argument("Invalid metric name");
  }
  for (auto& label_pair : constant_labels_) {
    auto& label_name = label_pair.first;
    if (!CheckLabelName(label_name, T::metric_type)) {
      throw std::invalid_argument("Invalid label name");
    }
  }
}

template <typename T>
T& Family<T>::Add(Labels labels, std::unique_ptr<T> object) {
  filter_labels(labels);

  std::lock_guard<std::mutex> lock{mutex_};

  auto insert_result =
      metrics_.insert(std::make_pair(labels, std::move(object)));

  if (insert_result.second) {
    // insertion took place, retroactively check for unlikely issues
    for (auto& label_pair : labels) {
      const auto& label_name = label_pair.first;
      if (!CheckLabelName(label_name, T::metric_type)) {
        metrics_.erase(insert_result.first);
        throw std::invalid_argument("Invalid label name");
      }
      if (constant_labels_.count(label_name)) {
        metrics_.erase(insert_result.first);
        throw std::invalid_argument("Duplicate label name");
      }
    }
  }

  auto& stored_object = insert_result.first->second;
  assert(stored_object);
  return *stored_object;
}

template <typename T>
void Family<T>::Remove(T* metric) {
  std::lock_guard<std::mutex> lock{mutex_};

  for (auto it = metrics_.begin(); it != metrics_.end(); ++it) {
    if (it->second.get() == metric) {
      metrics_.erase(it);
      break;
    }
  }
}

template <typename T>
bool Family<T>::Has(Labels labels) const {
  filter_labels(labels);

  std::lock_guard<std::mutex> lock{mutex_};
  auto count = metrics_.count(labels);
  return count != 0u;
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

  if (metrics_.empty()) {
    return {};
  }

  auto family = MetricFamily{};
  family.name = name_;
  family.help = help_;
  family.type = T::metric_type;
  family.metric.reserve(metrics_.size());
  for (const auto& m : metrics_) {
    family.metric.push_back(std::move(CollectMetric(m.first, m.second.get())));
  }
  return {family};
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
template class PROMETHEUS_CPP_CORE_EXPORT Family<Summary>;

}  // namespace prometheus

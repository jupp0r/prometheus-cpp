#include "prometheus/family.h"

#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const std::map<std::string, std::string>& constant_labels, const double& seconds)
    : name_(name), help_(help), constant_labels_(constant_labels), seconds_(seconds) {
  assert(CheckMetricName(name_));
}

template <typename T>
T& Family<T>::Add(const std::map<std::string, std::string>& labels,
                  std::unique_ptr<T> object) {
  auto hash = detail::hash_labels(labels);
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
#ifndef NDEBUG
    for (auto& label_pair : labels) {
      auto& label_name = label_pair.first;
      assert(CheckLabelName(label_name));
    }
#endif

    auto metric = metrics_.insert(std::make_pair(hash, std::move(object)));
    assert(metric.second);
    labels_.insert({hash, labels});
    labels_reverse_lookup_.insert({metric.first->second.get(), hash});
    return *(metric.first->second);
  }
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
const std::string& Family<T>::GetName() const {
  return name_;
}

template <typename T>
const std::map<std::string, std::string> Family<T>::GetConstantLabels() const {
  return constant_labels_;
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect() {
  const auto time = std::time(nullptr);
  return Collect(time);
}

template <typename T>
std::vector<MetricFamily> Family<T>::Collect(const std::time_t& time) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = MetricFamily{};
  family.name = name_;
  family.help = help_;
  family.type = T::metric_type;
  for (const auto& m : metrics_) {
    if (!m.second.get()->Expired(time, seconds_)) {
      family.metric.push_back(std::move(CollectMetric(m.first, m.second.get())));
    }
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

template class PROMETHEUS_CPP_CORE_EXPORT Family<Counter>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Gauge>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Histogram>;
template class PROMETHEUS_CPP_CORE_EXPORT Family<Summary>;

}  // namespace prometheus

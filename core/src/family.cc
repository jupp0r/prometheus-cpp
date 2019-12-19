#include "prometheus/family.h"

#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

#include <boost/regex.hpp>

namespace prometheus {

template <typename T>
Family<T>::Family(const std::string& name, const std::string& help,
                  const std::map<std::string, std::string>& constant_labels, 
                  const RetentionBehavior& retention_behavior)
    : name_(name), help_(help), constant_labels_(constant_labels), retention_behavior_(retention_behavior) {
  assert(CheckMetricName(name_));
}

template <typename T>
std::shared_ptr<T> Family<T>::Add(const std::map<std::string, std::string>& labels, std::shared_ptr<T> object) {
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
    return metrics_iter->second;
  } else {
#ifndef NDEBUG
    for (auto& label_pair : labels) {
      auto& label_name = label_pair.first;
      assert(CheckLabelName(label_name));
    }
#endif

    auto metric = metrics_.insert(std::make_pair(hash, object));
    assert(metric.second);
    labels_.insert({hash, labels});
    labels_reverse_lookup_.insert({metric.first->second, hash});
    object->has_family_ = true;
    return metric.first->second;
  }
}

template <typename T>
void Family<T>::Remove(std::shared_ptr<T> metric) {
  std::lock_guard<std::mutex> lock{mutex_};
  if (labels_reverse_lookup_.count(metric) == 0) {
    return;
  }

  auto hash = labels_reverse_lookup_.at(metric);
  metrics_.erase(hash);
  labels_.erase(hash);
  labels_reverse_lookup_.erase(metric);
  metric->has_family_ = false;
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
  auto family = MetricFamily{};
  std::vector<std::shared_ptr<T>> to_be_removed;
  {
    std::lock_guard<std::mutex> lock{mutex_};
    family.name = name_;
    family.help = help_;
    family.type = T::metric_type;
    for (const auto& m : metrics_) {
      if (!m.second->Expired()) {
        family.metric.push_back(std::move(CollectMetric(m.first, m.second)));
      } else if (retention_behavior_ == RetentionBehavior::Remove) {
        to_be_removed.push_back(m.second);
      }
    }
  } // end mutex
  for (auto metric: to_be_removed) {
    Remove(metric);
  }
  if (family.metric.empty()) {
    return {};
  } else {
    return {family};
  }
}

template <typename T>
bool Family<T>::UpdateRetentionTime(const double& retention_time, const std::string& re_name, const std::map<std::string, std::string>& re_labels, const bool& bump, const bool& debug) {
  /* Setup */
  bool modified(false);
  const boost::regex name_expr(re_name);
  if (debug) {  
    printf("UpdateRetentionTime: %s \n", re_name.c_str());
  }
  /* Check if the name is a match */
  if (boost::regex_match(name_, name_expr)) {
    for (auto metric: metrics_) {
      bool matched(true);
      auto hash = labels_reverse_lookup_.at(metric.second);
      for (const auto& re_label: re_labels) {
        /* Setup */
        bool found(false);
        const boost::regex key_expr(re_label.first);
        const boost::regex value_expr(re_label.second);        
        /* Check constant labels */
        for (const auto& label: constant_labels_) {
          if (boost::regex_match(label.first, key_expr) && boost::regex_match(label.second, value_expr)) {
            found = true;
            break;
          }
        }
        /* Check metric labels */
        for (const auto& label: labels_.at(hash)) {
          if (boost::regex_match(label.first, key_expr) && boost::regex_match(label.second, value_expr)) {
            found = true;
            break;
          }
        }
        /* Update matched status (matched = true if all labels up to this point are a match) */
        if (found) {
          matched = true;
        } else {
          matched = false;
          break;
        }
      } // End for loop (const auto& re_label: re_labels)
      if (matched) {
        /* Update retention time (name and labels match) */
        metric.second->UpdateRetentionTime(retention_time, bump);
        modified = true;
        if (debug) {
          printf("UpdateRetentionTime: name  = %s\n", name_.c_str());
          for (auto label: labels_.at(hash)) {
            printf("UpdateRetentionTime: label = <%s,%s>\n", label.first.c_str(), label.second.c_str());
          }
        }
      }
    } // End for loop (auto metric: metrics_)
  }
  return modified;
}

template <typename T>
ClientMetric Family<T>::CollectMetric(std::size_t hash, std::shared_ptr<T> metric) {
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

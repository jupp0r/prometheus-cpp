#include "registry.h"

namespace prometheus {

Registry::Registry(const std::map<std::string, std::string>& const_labels)
    : const_labels_(const_labels) {}

Family<Counter>* Registry::AddCounter(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto counter_family = new Family<Counter>(name, help, labels);
  collectables_.push_back(std::unique_ptr<Collectable>{counter_family});
  return counter_family;
}

Family<Gauge>* Registry::AddGauge(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto gauge_family = new Family<Gauge>(name, help, labels);
  collectables_.push_back(std::unique_ptr<Collectable>{gauge_family});
  return gauge_family;
}

Family<Histogram>* Registry::AddHistogram(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto histogram_family = new Family<Histogram>(name, help, labels);
  collectables_.push_back(std::unique_ptr<Collectable>{histogram_family});
  return histogram_family;
}

std::vector<io::prometheus::client::MetricFamily> Registry::Collect() {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<io::prometheus::client::MetricFamily>{};
  for (auto&& collectable : collectables_) {
    auto metrics = collectable->Collect();
    results.insert(results.end(), metrics.begin(), metrics.end());
  }

  for (auto&& metric_family : results) {
    for (auto&& metric : *metric_family.mutable_metric()) {
      for (auto&& const_label_pair : const_labels_) {
        auto label = metric.add_label();
        label->set_name(const_label_pair.first);
        label->set_value(const_label_pair.second);
      }
    }
  }

  return results;
}
}

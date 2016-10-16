#include "registry.h"

namespace prometheus {

Registry::Registry(const std::map<std::string, std::string>& constLabels)
    : constLabels_(constLabels) {}

Family<Counter>* Registry::add_counter(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  auto counterFamily = new Family<Counter>(name, help, labels);
  collectables_.push_back(std::unique_ptr<Collectable>{counterFamily});
  return counterFamily;
}

Family<Gauge>* Registry::add_gauge(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  auto gaugeFamily = new Family<Gauge>(name, help, labels);
  collectables_.push_back(std::unique_ptr<Collectable>{gaugeFamily});
  return gaugeFamily;
}

Family<Histogram>* Registry::add_histogram(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
    auto histogramFamily = new Family<Histogram>(name, help, labels);
    collectables_.push_back(std::unique_ptr<Collectable>{histogramFamily});
    return histogramFamily;
}

std::vector<io::prometheus::client::MetricFamily> Registry::collect() {
  auto results = std::vector<io::prometheus::client::MetricFamily>{};
  for (auto&& collectable : collectables_) {
    auto metrics = collectable->collect();
    results.insert(results.end(), metrics.begin(), metrics.end());
  }

  for (auto&& metricFamily : results) {
    for (auto&& metric : *metricFamily.mutable_metric()) {
      for (auto&& constLabelPair : constLabels_) {
        auto label = metric.add_label();
        label->set_name(constLabelPair.first);
        label->set_value(constLabelPair.second);
      }
    }
  }

  return results;
}
}

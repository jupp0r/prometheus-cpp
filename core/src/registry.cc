#include "prometheus/registry.h"

#include "prometheus/detail/future_std.h"

namespace prometheus {

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help,
                         const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = detail::make_unique<Family<T>>(name, help, labels);
  auto& ref = *family;
  collectables_.push_back(std::move(family));
  return ref;
}

Family<Counter>& Registry::AddCounter(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  return Add<Counter>(name, help, labels);
}

Family<Gauge>& Registry::AddGauge(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  return Add<Gauge>(name, help, labels);
}

Family<Histogram>& Registry::AddHistogram(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  return Add<Histogram>(name, help, labels);
}

Family<Summary>& Registry::AddSummary(
    const std::string& name, const std::string& help,
    const std::map<std::string, std::string>& labels) {
  return Add<Summary>(name, help, labels);
}

std::vector<MetricFamily> Registry::Collect() {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<MetricFamily>{};
  for (auto&& collectable : collectables_) {
    auto metrics = collectable->Collect();
    results.insert(results.end(), metrics.begin(), metrics.end());
  }

  return results;
}

}  // namespace prometheus

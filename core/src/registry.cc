#include "prometheus/registry.h"

#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

#include <iterator>

namespace prometheus {

Registry::Registry() = default;

Registry::~Registry() = default;

std::vector<MetricFamily> Registry::Collect() {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<MetricFamily>{};
  for (auto&& collectable : collectables_) {
    auto metrics = collectable->Collect();
    results.insert(results.end(), std::make_move_iterator(metrics.begin()),
                   std::make_move_iterator(metrics.end()));
  }

  return results;
}

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help,
                         const std::vector<std::string>& variable_labels,
                         const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = detail::make_unique<Family<T>>(name, help, variable_labels, labels);
  auto& ref = *family;
  collectables_.push_back(std::move(family));
  return ref;
}

template Family<Counter>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::vector<std::string>& variable_labels,
    const std::map<std::string, std::string>& labels);

template Family<Gauge>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::vector<std::string>& variable_labels,
    const std::map<std::string, std::string>& labels);

template Family<Summary>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::vector<std::string>& variable_labels,
    const std::map<std::string, std::string>& labels);

template Family<Histogram>& Registry::Add(
    const std::string& name, const std::string& help,
    const std::vector<std::string>& variable_labels,
    const std::map<std::string, std::string>& labels);

}  // namespace prometheus

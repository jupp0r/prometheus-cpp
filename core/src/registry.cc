#include "prometheus/registry.h"

namespace prometheus {

std::vector<MetricFamily> Registry::Collect() {
  const auto time = std::time(nullptr);
  return Collect(time);
}

std::vector<MetricFamily> Registry::Collect(std::time_t time) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<MetricFamily>{};
  for (auto&& collectable : collectables_) {
    auto metrics = collectable->Collect(time);
    results.insert(results.end(), metrics.begin(), metrics.end());
  }

  return results;
}

}  // namespace prometheus

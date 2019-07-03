#include "prometheus/registry.h"

#include <iterator>

namespace prometheus {

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

}  // namespace prometheus

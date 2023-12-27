#include "metrics_collector.h"

#include <iterator>

#include "prometheus/collectable.h"

namespace prometheus {
namespace detail {

void CollectMetrics(
    const Serializer& out,
    const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables) {
  auto collected_metrics = std::vector<MetricFamily>{};

  for (auto&& wcollectable : collectables) {
    auto collectable = wcollectable.lock();
    if (!collectable) {
      continue;
    }

    collectable->Collect(out);
  }
}

}  // namespace detail
}  // namespace prometheus

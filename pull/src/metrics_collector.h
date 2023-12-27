#pragma once

#include <memory>
#include <vector>

#include "prometheus/metric_family.h"
#include "prometheus/serializer.h"

namespace prometheus {
class Collectable;
namespace detail {
void CollectMetrics(
    const Serializer& out,
    const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables);
}  // namespace detail
}  // namespace prometheus

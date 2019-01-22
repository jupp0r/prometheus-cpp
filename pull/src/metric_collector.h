#pragma once

#include <memory>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/metric_family.h"

class CivetServer;

namespace prometheus {

    std::vector<prometheus::MetricFamily> CollectMetrics(const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables);

}  // namespace prometheus

#pragma once

#include <prometheus/client_metric.h>
#include <prometheus/metric_type.h>

#include <vector>

namespace prometheus {

struct MetricFamily {
  std::string name;
  std::string help;
  MetricType type = MetricType::Untyped;
  std::vector<ClientMetric> metric;
};

}  // namespace prometheus

#pragma once

#include <string>
#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/metric_family.h"

namespace prometheus {

class Serializer {
 public:
  virtual ~Serializer() = default;
  virtual std::string Serialize(const std::vector<MetricFamily>&) const = 0;
};
}  // namespace prometheus

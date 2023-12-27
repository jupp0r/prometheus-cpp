#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "prometheus/detail/core_export.h"
#include "prometheus/metric_family.h"

namespace prometheus {

class PROMETHEUS_CPP_CORE_EXPORT Serializer {
 public:
  virtual ~Serializer() = default;

  virtual void Serialize(const MetricFamily& family) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const ClientMetric& metric) const = 0;
};

}  // namespace prometheus

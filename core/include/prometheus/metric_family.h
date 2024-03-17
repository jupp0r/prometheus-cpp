#pragma once

#include <string>

#include "prometheus/detail/core_export.h"
#include "prometheus/metric_type.h"

namespace prometheus {

struct PROMETHEUS_CPP_CORE_EXPORT MetricFamily {
  std::string name;
  std::string help;
  MetricType type = MetricType::Untyped;
};
}  // namespace prometheus

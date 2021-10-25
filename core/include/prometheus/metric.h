#pragma once

#include <ctime>

#include "prometheus/detail/core_export.h"
#include "prometheus/metric_type.h"

namespace prometheus {

/// \brief The base metric.
///
class PROMETHEUS_CPP_CORE_EXPORT Metric {
 public:
  static const MetricType metric_type{MetricType::Untyped};

  Metric();
  virtual ~Metric() = default;

  bool Expired(time_t current_ts) const;

 protected:
  void UpdateTS();

 private:
  time_t last_update_ts_;
};

}  // namespace prometheus

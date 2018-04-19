#pragma once

#include "prometheus/client_metric.h"

namespace prometheus {

class Metric {
 public:
  virtual ~Metric() = default;
  virtual ClientMetric Collect() = 0;
};
}

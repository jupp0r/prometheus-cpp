#pragma once

#include "metrics.pb.h"

namespace prometheus {

class Metric {
 public:
  virtual ~Metric() = default;
  virtual io::prometheus::client::Metric Collect() = 0;
};
}

#pragma once

#include "cpp/metrics.pb.h"

namespace prometheus {

class Metric {
 public:
  virtual ~Metric() = default;
  virtual io::prometheus::client::Metric collect() = 0;
};
}

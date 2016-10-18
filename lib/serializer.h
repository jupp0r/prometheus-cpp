#pragma once

#include <string>
#include <vector>

#include "cpp/metrics.pb.h"

namespace prometheus {

class Serializer {
 public:
  virtual ~Serializer() = default;
  virtual std::string Serialize(
      const std::vector<io::prometheus::client::MetricFamily>&) = 0;
};
}

#pragma once

#include <vector>

namespace prometheus {
struct MetricFamily;
}  // namespace prometheus

namespace prometheus {

class Collectable {
 public:
  virtual ~Collectable() = default;
  virtual std::vector<MetricFamily> Collect() = 0;
};

}  // namespace prometheus

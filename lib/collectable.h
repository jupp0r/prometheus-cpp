#pragma once

#include <vector>

namespace io {
namespace prometheus {
namespace client {
class MetricFamily;
}
}
}

namespace prometheus {

class Collectable {
 public:
  virtual ~Collectable() = default;
  virtual std::vector<io::prometheus::client::MetricFamily> collect() = 0;
};
}

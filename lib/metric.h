#pragma once

namespace io {
namespace prometheus {
namespace client {
class Metric;
}
}
}

namespace prometheus {

class Metric {
  public:
  virtual ~Metric() = default;
  virtual io::prometheus::client::Metric collect() = 0;
};
}

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "metric.h"

namespace io {
namespace prometheus {
namespace client {
class Metric;
class LabelPair;
}
}
}

namespace prometheus {

class LabelDecorator : Metric {
public:
  LabelDecorator(std::vector<std::pair<std::string, std::string>> labels,
                 std::unique_ptr<Metric> metric);

  io::prometheus::client::Metric collect();

private:
  const std::vector<std::pair<std::string, std::string>> labels_;
  std::unique_ptr<Metric> metric_;
};
}

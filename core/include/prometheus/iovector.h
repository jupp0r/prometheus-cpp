#pragma once

#include <string>
#include <vector>

#include "prometheus/detail/core_export.h"

namespace prometheus {

struct PROMETHEUS_CPP_CORE_EXPORT IOVector {
  std::vector<std::string> data;
};

}  // namespace prometheus

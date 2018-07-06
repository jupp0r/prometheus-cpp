#pragma once

#include <string>
#include <vector>

#include "prometheus/metric_family.h"
#include "prometheus/serializer.h"

namespace prometheus {

class TextSerializer : public Serializer {
 public:
  std::string Serialize(const std::vector<MetricFamily>& metrics);
};
}

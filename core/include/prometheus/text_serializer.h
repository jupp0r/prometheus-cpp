#pragma once

#include <string>
#include <vector>

#include "prometheus/metric_family.h"
#include "prometheus/serializer.h"

namespace prometheus {

class TextSerializer : public Serializer {
 public:
  void Serialize(std::ostream& out,
                 const std::vector<MetricFamily>& metrics) const override;
};
}

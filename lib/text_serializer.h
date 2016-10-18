#pragma once

#include <string>
#include <vector>

#include "cpp/metrics.pb.h"

#include "serializer.h"

namespace prometheus {

class TextSerializer : public Serializer {
 public:
  std::string Serialize(
      const std::vector<io::prometheus::client::MetricFamily>& metrics);
};
}

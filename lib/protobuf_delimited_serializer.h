#pragma once

#include <string>
#include <vector>

#include "metrics.pb.h"

#include "serializer.h"

namespace prometheus {

class ProtobufDelimitedSerializer : public Serializer {
 public:
  std::string Serialize(
      const std::vector<io::prometheus::client::MetricFamily>& metrics);
};
}

#include "text_serializer.h"

namespace prometheus {

std::string TextSerializer::Serialize(
    const std::vector<io::prometheus::client::MetricFamily>& metrics) {
  auto result = std::string{};
  for (auto&& metric : metrics) {
    result += metric.DebugString() + "\n";
  }
  return result;
}
}

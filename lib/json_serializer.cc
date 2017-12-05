#include <iostream>
#include <sstream>

#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/message_differencer.h>

#include "prometheus/json_serializer.h"

namespace prometheus {

std::string JsonSerializer::Serialize(
    const std::vector<io::prometheus::client::MetricFamily>& metrics) {
  using google::protobuf::util::MessageDifferencer;

  std::stringstream ss;
  ss << "[";

  for (auto&& metric : metrics) {
    std::string result;
    google::protobuf::util::MessageToJsonString(
        metric, &result, google::protobuf::util::JsonPrintOptions());
    ss << result;
    if (!MessageDifferencer::Equals(metric, metrics.back())) {
      ss << ",";
    }
  }
  ss << "]";
  return ss.str();
}
}

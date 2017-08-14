#pragma once

#include <string>
#include <vector>

#include "metrics.pb.h"
#include "serializer.h"

namespace prometheus {

class TextSerializer : public Serializer {
 public:
  std::string Serialize(
      const std::vector<io::prometheus::client::MetricFamily>& metrics);

 private:
  static std::string CovertToString(const io::prometheus::client::MetricFamily& metric_family);
  static std::string LabelStr(io::prometheus::client::Metric& m); 
  static std::string SingleMetricStr(io::prometheus::client::Metric& m,
		const std::string& metric_name, 
		const io::prometheus::client::MetricType type);
};
}

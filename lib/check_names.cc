#include <regex>

#include <prometheus/check_names.h>

namespace prometheus {
bool CheckMetricName(const std::string& name) {
  // see https://prometheus.io/docs/concepts/data_model/
  auto reserved_for_internal_purposes = name.compare(0, 2, "__") == 0;
  static const std::regex metric_name_regex("[a-zA-Z_:][a-zA-Z0-9_:]*");
  return std::regex_match(name, metric_name_regex) &&
         !reserved_for_internal_purposes;
}

bool CheckLabelName(const std::string& name) {
  auto reserved_for_internal_purposes = name.compare(0, 2, "__") == 0;
  // see https://prometheus.io/docs/concepts/data_model/
  static const std::regex label_name_regex("[a-zA-Z_][a-zA-Z0-9_]*");
  return std::regex_match(name, label_name_regex) &&
         !reserved_for_internal_purposes;
  ;
}
}

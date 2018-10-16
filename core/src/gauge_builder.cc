#include "prometheus/gauge_builder.h"
#include "prometheus/registry.h"

namespace prometheus {

detail::GaugeBuilder BuildGauge() { return {}; }

namespace detail {

GaugeBuilder& GaugeBuilder::Labels(
    const std::map<std::string, std::string>& labels) {
  labels_ = labels;
  return *this;
}

GaugeBuilder& GaugeBuilder::Name(const std::string& name) {
  name_ = name;
  return *this;
}

GaugeBuilder& GaugeBuilder::Help(const std::string& help) {
  help_ = help;
  return *this;
}

Family<Gauge>& GaugeBuilder::Register(Registry& registry) {
  return registry.AddGauge(name_, help_, labels_);
}
}  // namespace detail
}  // namespace prometheus

#include "prometheus/summary_builder.h"

#include "prometheus/registry.h"

namespace prometheus {

detail::SummaryBuilder BuildSummary() { return {}; }

namespace detail {

SummaryBuilder& SummaryBuilder::Labels(
    const std::map<std::string, std::string>& labels) {
  labels_ = labels;
  return *this;
}

SummaryBuilder& SummaryBuilder::Name(const std::string& name) {
  name_ = name;
  return *this;
}

SummaryBuilder& SummaryBuilder::Help(const std::string& help) {
  help_ = help;
  return *this;
}

Family<Summary>& SummaryBuilder::Register(Registry& registry) {
  return registry.AddSummary(name_, help_, labels_);
}
}  // namespace detail
}  // namespace prometheus

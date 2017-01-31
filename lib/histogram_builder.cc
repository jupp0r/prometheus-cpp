#include "prometheus/gauge_builder.h"
#include "prometheus/registry.h"

namespace prometheus {

detail::HistogramBuilder BuildHistogram() { return {}; }

namespace detail {

HistogramBuilder& HistogramBuilder::Labels(
    const std::map<std::string, std::string>& labels) {
  labels_ = labels;
  return *this;
}

HistogramBuilder& HistogramBuilder::Name(const std::string& name) {
  name_ = name;
  return *this;
}

HistogramBuilder& HistogramBuilder::Help(const std::string& help) {
  help_ = help;
  return *this;
}

HistogramBuilder& HistogramBuilder::Buckets(
    const std::vector<double>& buckets) {
  buckets_ = buckets;
  return *this;
}

Family<Histogram>& HistogramBuilder::Register(Registry& registry) {
  return registry.AddHistogram(name_, help_, labels_);
}
}
}

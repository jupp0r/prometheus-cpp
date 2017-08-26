#include <sstream>

#include "text_serializer.h"

namespace {

template <typename T>
void renderKVPairs(const T& pairs, std::stringstream& ss) {
  auto size = std::distance(pairs.begin(), pairs.end());
  for (std::size_t i = 0; i < size; i++) {
    auto& label_pair = pairs[i];
    ss << label_pair.name() << "=\"" << label_pair.value() << "\"";
    if (i != size - 1) {
      ss << ",";
    }
  }
}

void renderLabels(const io::prometheus::client::Metric& metric,
                  std::stringstream& ss) {
  if (metric.label_size() == 0) {
    return;
  }

  ss << "{";
  renderKVPairs(metric.label(), ss);
  ss << "}";
}

void renderFloatValue(double value, std::stringstream& ss) {
  std::string rendered;
  if (value == std::numeric_limits<double>::infinity()) {
    rendered = "+Inf";
  } else if (value == -std::numeric_limits<double>::infinity()) {
    rendered = "-Inf";
  } else if (value == std::numeric_limits<double>::quiet_NaN() ||
             value == std::numeric_limits<double>::signaling_NaN()) {
    rendered = "NaN";
  } else {
    std::ostringstream strs;
    strs << value;
    rendered = strs.str();
  }
  ss << rendered;
}

void renderSingularValue(const io::prometheus::client::Metric& metric,
                         const std::string& name, std::stringstream& ss) {
  ss << name;
  renderLabels(metric, ss);
  ss << " ";

  if (metric.has_counter()) {
    renderFloatValue(metric.counter().value(), ss);
  }
  if (metric.has_gauge()) {
    renderFloatValue(metric.gauge().value(), ss);
  }

  ss << " " << metric.timestamp_ms();
  ss << std::endl;
}

void renderHistogram(const io::prometheus::client::Metric& metric,
                     const std::string& name, std::stringstream& ss) {
  const auto& histogram = metric.histogram();

  for (auto& bucket : histogram.bucket()) {
    ss << name << "{";
    renderKVPairs(metric.label(), ss);
    ss << ","
       << "le=\"" << bucket.upper_bound() << "\"}" << std::endl;
  }

  ss << name << "_sum";
  renderLabels(metric, ss);
  ss << " " << histogram.sample_sum() << " " << metric.timestamp_ms()
     << std::endl;

  ss << name << "_count";
  renderLabels(metric, ss);
  ss << " " << histogram.sample_count() << " " << metric.timestamp_ms()
     << std::endl;
}

void renderMetric(const io::prometheus::client::Metric& metric,
                  const std::string& name, std::stringstream& ss) {
  if (metric.has_histogram()) {
    renderHistogram(metric, name, ss);
    return;
  }

  if (metric.has_counter() || metric.has_gauge()) {
    renderSingularValue(metric, name, ss);
  }
}

void renderMetricFamily(const io::prometheus::client::MetricFamily& family,
                        std::stringstream& ss) {
  using io::prometheus::client::MetricType;
  using io::prometheus::client::COUNTER;
  using io::prometheus::client::GAUGE;
  using io::prometheus::client::HISTOGRAM;

  const auto& name = family.name();
  const auto& help = family.help();

  static const std::map<MetricType, std::string> type = {
      {COUNTER, "counter"}, {GAUGE, "gauge"}, {HISTOGRAM, "histogram"}};

  ss << "# HELP " << name << " " << help << std::endl;
  ss << "# TYPE " << name << " " << type.at(family.type()) << std::endl;
  for (auto&& metric : family.metric()) {
    renderMetric(metric, name, ss);
  }
}
}  // namespace

namespace prometheus {

std::string TextSerializer::Serialize(
    const std::vector<io::prometheus::client::MetricFamily>& metrics) {
  auto result = std::stringstream{};
  for (auto&& family : metrics) {
    renderMetricFamily(family, result);
  }
  return result.str();
}
}  // namespace prometheus

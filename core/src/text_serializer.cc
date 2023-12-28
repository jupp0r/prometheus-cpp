#include "prometheus/text_serializer.h"

#include <cmath>
#include <limits>
#include <locale>
#include <ostream>
#include <sstream>
#include <string>

#include "prometheus/client_metric.h"
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

namespace prometheus {

namespace {

// Write a double as a string, with proper formatting for infinity and NaN
void WriteValue(std::ostream& out, double value) {
  if (std::isnan(value)) {
    out << "Nan";
  } else if (std::isinf(value)) {
    out << (value < 0 ? "-Inf" : "+Inf");
  } else {
    out << value;
  }
}

void WriteValue(std::ostream& out, const std::string& value) {
  for (auto c : value) {
    switch (c) {
      case '\n':
        out << '\\' << 'n';
        break;

      case '\\':
        out << '\\' << c;
        break;

      case '"':
        out << '\\' << c;
        break;

      default:
        out << c;
        break;
    }
  }
}

// Write a line header: metric name and labels
template <typename T = std::string>
void WriteHead(std::ostream& out, const MetricFamily& family,
               const Labels& constantLabels, const Labels& metricLabels,
               const std::string& suffix = "",
               const std::string& extraLabelName = "",
               const T& extraLabelValue = T()) {
  out << family.name << suffix;
  if (!constantLabels.empty() || !metricLabels.empty() ||
      !extraLabelName.empty()) {
    out << "{";
    const char* prefix = "";

    for (auto& lp : constantLabels) {
      out << prefix << lp.first << "=\"";
      WriteValue(out, lp.second);
      out << "\"";
      prefix = ",";
    }
    for (auto& lp : metricLabels) {
      out << prefix << lp.first << "=\"";
      WriteValue(out, lp.second);
      out << "\"";
      prefix = ",";
    }
    if (!extraLabelName.empty()) {
      out << prefix << extraLabelName << "=\"";
      WriteValue(out, extraLabelValue);
      out << "\"";
    }
    out << "}";
  }
  out << " ";
}

// Write a line trailer: timestamp
void WriteTail(std::ostream& out, const CoreMetric& metric) {
  if (metric.timestamp_ms != 0) {
    out << " " << metric.timestamp_ms;
  }
  out << "\n";
}
}  // namespace

TextSerializer::TextSerializer(IOVector& ioVector) : ioVector_(ioVector) {}

void TextSerializer::Serialize(const MetricFamily& family) const {
  std::ostringstream out;

  if (!family.help.empty()) {
    out << "# HELP " << family.name << " " << family.help << "\n";
  }
  switch (family.type) {
    case MetricType::Counter:
      out << "# TYPE " << family.name << " counter\n";
      break;
    case MetricType::Gauge:
      out << "# TYPE " << family.name << " gauge\n";
      break;
    // info is not handled by prometheus, we use gauge as workaround
    // (https://github.com/OpenObservability/OpenMetrics/blob/98ae26c87b1c3bcf937909a880b32c8be643cc9b/specification/OpenMetrics.md#info-1)
    case MetricType::Info:
      out << "# TYPE " << family.name << " gauge\n";
      break;
    case MetricType::Summary:
      out << "# TYPE " << family.name << " summary\n";
      break;
    case MetricType::Untyped:
      out << "# TYPE " << family.name << " untyped\n";
      break;
    case MetricType::Histogram:
      out << "# TYPE " << family.name << " histogram\n";
      break;
  }

  Add(out);
}

void TextSerializer::Serialize(const MetricFamily& family,
                               const Labels& constantLabels,
                               const Labels& metricLabels,
                               const CounterMetric& metric) const {
  std::ostringstream out;

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  WriteHead(out, family, constantLabels, metricLabels);
  WriteValue(out, metric.value);
  WriteTail(out, metric);

  Add(out);
}

void TextSerializer::Serialize(const MetricFamily& family,
                               const Labels& constantLabels,
                               const Labels& metricLabels,
                               const GaugeMetric& metric) const {
  std::ostringstream out;

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  WriteHead(out, family, constantLabels, metricLabels);
  WriteValue(out, metric.value);
  WriteTail(out, metric);

  Add(out);
}

void TextSerializer::Serialize(const MetricFamily& family,
                               const Labels& constantLabels,
                               const Labels& metricLabels,
                               const InfoMetric& metric) const {
  std::ostringstream out;

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  WriteHead(out, family, constantLabels, metricLabels, "_info");
  WriteValue(out, metric.value);
  WriteTail(out, metric);

  Add(out);
}

void TextSerializer::Serialize(const MetricFamily& family,
                               const Labels& constantLabels,
                               const Labels& metricLabels,
                               const SummaryMetric& metric) const {
  std::ostringstream out;

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  WriteHead(out, family, constantLabels, metricLabels, "_count");
  out << metric.sample_count;
  WriteTail(out, metric);

  WriteHead(out, family, constantLabels, metricLabels, "_sum");
  WriteValue(out, metric.sample_sum);
  WriteTail(out, metric);

  for (auto& q : metric.quantile) {
    WriteHead(out, family, constantLabels, metricLabels, "", "quantile",
              q.quantile);
    WriteValue(out, q.value);
    WriteTail(out, metric);
  }

  Add(out);
}

void TextSerializer::Serialize(const MetricFamily& family,
                               const Labels& constantLabels,
                               const Labels& metricLabels,
                               const HistogramMetric& metric) const {
  std::ostringstream out;

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  WriteHead(out, family, constantLabels, metricLabels, "_count");
  out << metric.sample_count;
  WriteTail(out, metric);

  WriteHead(out, family, constantLabels, metricLabels, "_sum");
  WriteValue(out, metric.sample_sum);
  WriteTail(out, metric);

  double last = -std::numeric_limits<double>::infinity();
  for (auto& b : metric.bucket) {
    WriteHead(out, family, constantLabels, metricLabels, "_bucket", "le",
              b.upper_bound);
    last = b.upper_bound;
    out << b.cumulative_count;
    WriteTail(out, metric);
  }

  if (last != std::numeric_limits<double>::infinity()) {
    WriteHead(out, family, constantLabels, metricLabels, "_bucket", "le",
              "+Inf");
    out << metric.sample_count;
    WriteTail(out, metric);
  }

  Add(out);
}

void TextSerializer::Serialize(const MetricFamily& family,
                               const Labels& constantLabels,
                               const Labels& metricLabels,
                               const UntypedMetric& metric) const {
  std::ostringstream out;

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  WriteHead(out, family, constantLabels, metricLabels);
  WriteValue(out, metric.value);
  WriteTail(out, metric);

  Add(out);
}

void TextSerializer::Add(const std::ostringstream& stream) const {
  std::string str = stream.str();

  std::size_t size = str.size();
  std::size_t offset = 0;

  while (size > 0) {
    if (ioVector_.data.empty() || ioVector_.data.back().size() >= chunkSize_) {
      ioVector_.data.emplace_back();
      ioVector_.data.back().reserve(chunkSize_);
    }
    auto&& chunk = ioVector_.data.back();
    std::size_t toAdd = std::min(size, chunkSize_ - chunk.size());
    chunk.insert(chunk.end(), str.data() + offset, str.data() + offset + toAdd);

    size -= toAdd;
    offset += toAdd;
  }
}

}  // namespace prometheus

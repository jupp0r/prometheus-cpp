#include "prometheus/text_serializer.h"

#include <gmock/gmock.h>

#include <cmath>
#include <limits>

#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

class TextSerializerTest : public testing::Test {
 public:
  std::string Serialize(MetricType type) const {
    MetricFamily metricFamily;
    metricFamily.name = name;
    metricFamily.help = "my metric help text";
    metricFamily.type = type;
    metricFamily.metric = std::vector<ClientMetric>{metric};

    std::vector<MetricFamily> families{metricFamily};

    return textSerializer.Serialize(families);
  }

  const std::string name = "my_metric";
  ClientMetric metric;
  TextSerializer textSerializer;
};

TEST_F(TextSerializerTest, shouldSerializeNotANumber) {
  metric.gauge.value = std::nan("");
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::HasSubstr(name + " Nan"));
}

TEST_F(TextSerializerTest, shouldSerializeNegativeInfinity) {
  metric.gauge.value = -std::numeric_limits<double>::infinity();
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::HasSubstr(name + " -Inf"));
}

TEST_F(TextSerializerTest, shouldSerializePositiveInfinity) {
  metric.gauge.value = std::numeric_limits<double>::infinity();
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::HasSubstr(name + " +Inf"));
}

TEST_F(TextSerializerTest, shouldEscapeBackslash) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\\v"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\\v\"}"));
}

TEST_F(TextSerializerTest, shouldEscapeNewline) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\nv"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\nv\"}"));
}

TEST_F(TextSerializerTest, shouldEscapeDoubleQuote) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\"v"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\"v\"}"));
}

TEST_F(TextSerializerTest, shouldSerializeUntyped) {
  metric.untyped.value = 64.0;

  const auto serialized = Serialize(MetricType::Untyped);
  EXPECT_THAT(serialized, testing::HasSubstr(name + " 64.000000"));
}

TEST_F(TextSerializerTest, shouldSerializeHistogram) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(200);
  metric = histogram.Collect();

  const auto serialized = Serialize(MetricType::Histogram);
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 200.00000"));
  EXPECT_THAT(serialized,
              testing::HasSubstr(name + "_bucket{le=\"1.000000\"} 1"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_bucket{le=\"+Inf\"} 2"));
}

TEST_F(TextSerializerTest, shouldSerializeSummary) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(200);
  metric = summary.Collect();

  const auto serialized = Serialize(MetricType::Summary);
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 200.00000"));
  EXPECT_THAT(serialized,
              testing::HasSubstr(name + "{quantile=\"0.500000\"} 0.000000"));
}

}  // namespace
}  // namespace prometheus

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "mock_serializer.h"
#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/info.h"
#include "prometheus/labels.h"
#include "prometheus/metric_family.h"
#include "prometheus/registry.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::InSequence;
using ::testing::UnorderedElementsAreArray;

class BuilderTest : public testing::Test {
 protected:
  std::vector<ClientMetric::Label> getExpectedLabels() {
    std::vector<ClientMetric::Label> labels;

    auto gen = [](const std::pair<const std::string, std::string>& p) {
      return ClientMetric::Label{p.first, p.second};
    };

    std::transform(std::begin(const_labels), std::end(const_labels),
                   std::back_inserter(labels), gen);
    std::transform(std::begin(more_labels), std::end(more_labels),
                   std::back_inserter(labels), gen);

    return labels;
  }

  void verifyCollectedLabels() {
    MockSerializer serializer;

    {
      InSequence seq;

      EXPECT_CALL(serializer,
                  SerializeHelp(AllOf(Field(&MetricFamily::name, name),
                                      Field(&MetricFamily::help, help))));
      EXPECT_CALL(
          serializer,
          SerializeMetrics(
              _, AllOf(Field(&ClientMetric::label,
                             UnorderedElementsAreArray(expected_labels)))));
    }

    registry.Collect(serializer);
  }

  Registry registry;

  const std::string name = "some_name";
  const std::string help = "Additional description.";
  const Labels const_labels = {{"key", "value"}};
  const Labels more_labels = {{"name", "test"}};
  const std::vector<ClientMetric::Label> expected_labels = getExpectedLabels();
};

TEST_F(BuilderTest, build_counter) {
  auto& family = BuildCounter()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels);

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_gauge) {
  auto& family = BuildGauge()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels);

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_histogram) {
  auto& family = BuildHistogram()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels, Histogram::BucketBoundaries{1, 2});

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_info) {
  auto& family =
      BuildInfo().Name(name).Help(help).Labels(const_labels).Register(registry);
  family.Add(more_labels);

  verifyCollectedLabels();
}

TEST_F(BuilderTest, build_summary) {
  auto& family = BuildSummary()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels, Summary::Quantiles{});

  verifyCollectedLabels();
}

}  // namespace
}  // namespace prometheus

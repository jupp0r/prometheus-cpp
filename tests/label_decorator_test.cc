#include <memory>

#include <gmock/gmock.h>
#include "cpp/metrics.pb.h"
#include "lib/label_decorator.h"
#include "mock_metric.h"

using namespace testing;
using namespace prometheus;

class LabelDecoratorTest : public Test {};

TEST_F(LabelDecoratorTest, initialize_without_labels) {
  auto metricPtr = std::unique_ptr<MockMetric>(new NiceMock<MockMetric>());
  auto metric = metricPtr.get();
  ON_CALL(*metric, collect())
      .WillByDefault(Return(io::prometheus::client::Metric{}));
  auto labelDecorator = LabelDecorator{{}, std::move(metricPtr)};

  auto collected = labelDecorator.collect();
  EXPECT_THAT(collected.label_size(), Eq(0));
}

TEST_F(LabelDecoratorTest, initialize_with_labels) {
  auto metric = std::unique_ptr<MockMetric>(new NiceMock<MockMetric>());
  auto metricWithLabels = io::prometheus::client::Metric{};

  auto firstLabel = metricWithLabels.add_label();
  firstLabel->set_name("foo");
  firstLabel->set_value("bar");
  auto secondLabel = metricWithLabels.add_label();
  secondLabel->set_name("boo");
  secondLabel->set_value("baz");

  ON_CALL(*metric, collect())
      .WillByDefault(Return(io::prometheus::client::Metric{}));
  auto labelDecorator =
      LabelDecorator{{{firstLabel->name(), firstLabel->value()},
                      {secondLabel->name(), secondLabel->value()}},
                     std::move(metric)};

  auto collected = labelDecorator.collect();
  EXPECT_EQ(collected.DebugString(), metricWithLabels.DebugString());
}

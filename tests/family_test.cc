#include <memory>

#include <gmock/gmock.h>

#include <prometheus/exposer.h>
#include <prometheus/family.h>
#include <prometheus/histogram.h>

#include "metrics.pb.h"

using namespace testing;
using namespace prometheus;

class FamilyTest : public Test {};

namespace io {
namespace prometheus {
namespace client {
bool operator==(const io::prometheus::client::LabelPair& a,
                const io::prometheus::client::LabelPair& b) {
  return std::tie(a.name(), a.value()) == std::tie(b.name(), b.value());
}
}
}
}

TEST_F(FamilyTest, labels) {
  auto const_label = io::prometheus::client::LabelPair{};
  const_label.set_name("component");
  const_label.set_value("test");
  auto dynamic_label = io::prometheus::client::LabelPair{};
  dynamic_label.set_name("status");
  dynamic_label.set_value("200");

  Family<Counter> family{"total_requests",
                         "Counts all requests",
                         {{const_label.name(), const_label.value()}}};
  family.Add({{dynamic_label.name(), dynamic_label.value()}});
  auto collected = family.Collect();
  ASSERT_GE(collected.size(), 1);
  ASSERT_GE(collected[0].metric_size(), 1);
  EXPECT_THAT(collected[0].metric(0).label(),
              ElementsAre(const_label, dynamic_label));
}

TEST_F(FamilyTest, counter_value) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter = family.Add({});
  counter.Increment();
  auto collected = family.Collect();
  ASSERT_GE(collected.size(), 1);
  ASSERT_GE(collected[0].metric_size(), 1);
  EXPECT_THAT(collected[0].metric(0).counter().value(), Eq(1));
}

TEST_F(FamilyTest, remove) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter1 = family.Add({{"name", "counter1"}});
  family.Add({{"name", "counter2"}});
  family.Remove(&counter1);
  auto collected = family.Collect();
  ASSERT_GE(collected.size(), 1);
  EXPECT_EQ(collected[0].metric_size(), 1);
}

TEST_F(FamilyTest, Histogram) {
  Family<Histogram> family{"request_latency", "Latency Histogram", {}};
  auto& histogram1 = family.Add({{"name", "histogram1"}},
                               Histogram::BucketBoundaries{0, 1, 2});
  histogram1.Observe(0);
  auto collected = family.Collect();
  ASSERT_EQ(collected.size(), 1);
  ASSERT_GE(collected[0].metric_size(), 1);
  ASSERT_TRUE(collected[0].metric(0).has_histogram());
  EXPECT_THAT(collected[0].metric(0).histogram().sample_count(), Eq(1));
}

#include <memory>

#include <gmock/gmock.h>
#include "cpp/metrics.pb.h"
#include "lib/counter.h"
#include "lib/family.h"
#include "lib/histogram.h"

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
  auto constLabel = io::prometheus::client::LabelPair{};
  constLabel.set_name("component");
  constLabel.set_value("test");
  auto dynamicLabel = io::prometheus::client::LabelPair{};
  dynamicLabel.set_name("status");
  dynamicLabel.set_value("200");

  auto family = Family<Counter>{"total_requests",
                                "Counts all requests",
                                {{constLabel.name(), constLabel.value()}}};
  family.add({{dynamicLabel.name(), dynamicLabel.value()}});
  auto collected = family.collect();
  ASSERT_GE(collected.size(), 1);
  ASSERT_GE(collected[0].metric_size(), 1);
  EXPECT_THAT(collected[0].metric(0).label(),
              ElementsAre(constLabel, dynamicLabel));
}

TEST_F(FamilyTest, counter_value) {
  auto family = Family<Counter>{"total_requests", "Counts all requests", {}};
  auto counter = family.add({});
  counter->inc();
  auto collected = family.collect();
  ASSERT_GE(collected.size(), 1);
  ASSERT_GE(collected[0].metric_size(), 1);
  EXPECT_THAT(collected[0].metric(0).counter().value(), Eq(1));
}

TEST_F(FamilyTest, remove) {
  auto family = Family<Counter>{"total_requests", "Counts all requests", {}};
  auto counter1 = family.add({{"name", "counter1"}});
  family.add({{"name", "counter2"}});
  family.remove(counter1);
  auto collected = family.collect();
  ASSERT_GE(collected.size(), 1);
  EXPECT_EQ(collected[0].metric_size(), 1);
}

TEST_F(FamilyTest, histogram) {
  auto family = Family<Histogram>{"request_latency", "Latency Histogram", {}};
  auto histogram1 = family.add({{"name", "histogram1"}}, Histogram::BucketBoundaries{0,1,2});
  histogram1->observe(0);
  auto collected = family.collect();
  ASSERT_EQ(collected.size(), 1);
  ASSERT_GE(collected[0].metric_size(), 1);
  ASSERT_TRUE(collected[0].metric(0).has_histogram());
  EXPECT_THAT(collected[0].metric(0).histogram().sample_count(), Eq(1));
}

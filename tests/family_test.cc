#include <memory>

#include <gmock/gmock.h>
#include "cpp/metrics.pb.h"
#include "lib/counter.h"
#include "lib/family.h"

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
